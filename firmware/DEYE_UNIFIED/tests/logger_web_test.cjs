const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync(path.join(__dirname, '../web_logger_discovery.h'), 'utf8');
const script = source.match(/<script>([\s\S]*?)<\/script>/)[1];
const fields = {host: {value: 'old-host'}, serial: {value: '2712345678'}};
const status = {textContent: ''};
const button = {addEventListener(event, cb) { this.click = cb; }};
const form = {querySelector(selector) { return fields[selector.includes('host') ? 'host' : 'serial']; }};
const calls = [];
let state = {state: 0, generation: 0};
let failure = false, startFailure = false, active = false;
const context = {
  token: 'test-csrf', URLSearchParams,
  document: {querySelector() { return form; }, getElementById(id) { return id === 'logger-status' ? status : button; }},
  setInterval() { active = true; return 1; }, clearInterval() { active = false; },
  fetch: async (url, options) => {
    calls.push({url, options});
    if (failure) throw Error('Network lost');
    if (url.endsWith('/start')) return {ok: !startFailure, text: async () => 'Already running'};
    return {ok: true, json: async () => state};
  }
};
vm.createContext(context);
vm.runInContext(script, context);
async function run() {
  await new Promise(resolve => setImmediate(resolve));
  state = {state: 1, scanned: 0, total: 0};
  await button.click();
  const start = calls.find(call => call.url.endsWith('/start'));
  assert.equal(start.options.method, 'POST');
  assert.equal(start.options.headers['X-CSRF-Token'], 'test-csrf');
  assert.equal(start.options.body.get('serial'), '2712345678');
  assert(active);
  state = {state: 2, phase: 2, scanned: 10, total: 510};
  await vm.runInContext('pollLogger()', context);
  assert.match(status.textContent, /TCP\/V5 : 10 \/ 510/);
  state = {state: 3, generation: 1, host: '192.168.1.12', serial: 2712345678};
  await vm.runInContext('pollLogger()', context);
  assert(!active);
  assert.equal(fields.host.value, '192.168.1.12');
  assert.equal(fields.serial.value, 2712345678);
  assert.match(status.textContent, /Sauvegarder/);
  fields.host.value = 'user-edited';
  await vm.runInContext('pollLogger()', context);
  assert.equal(fields.host.value, 'user-edited');
  state = {...state, generation: 2, host: '192.168.1.13'};
  await vm.runInContext('pollLogger()', context);
  assert.equal(fields.host.value, '192.168.1.13');
  state = {...state, automatic: true, restart_pending: true, host: '192.168.1.77'};
  await vm.runInContext('pollLogger()', context);
  assert.match(status.textContent, /serie conserve.*Redemarrage/);
  assert.equal(fields.host.value, '192.168.1.13'); // Automatic discovery does not overwrite an open form.
  state = {...state, restart_pending: false, save_failed: true};
  await vm.runInContext('pollLogger()', context);
  assert.match(status.textContent, /aucun redemarrage/);
  state = {state: 4}; await vm.runInContext('pollLogger()', context);
  assert.match(status.textContent, /Aucun logger/);
  state = {state: 5}; await vm.runInContext('pollLogger()', context);
  assert.match(status.textContent, /interrompue/);
  startFailure = true; await button.click();
  assert.equal(status.textContent, 'Already running');
  failure = true; await vm.runInContext('pollLogger()', context);
  assert.equal(status.textContent, 'Network lost'); assert(!active);
  assert(!calls.some(call => call.url === '/deye'));
  console.log('Production logger Web tests passed.');
}
run().catch(error => { console.error(error); process.exitCode = 1; });
