const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');

const source = fs.readFileSync(path.join(__dirname, '../web_vetronic.h'), 'utf8');
const pieces = [...source.matchAll(/R"HTML\(([\s\S]*?)\)HTML"/g)].map(m => m[1]);
assert.equal(pieces.length, 2);
const page = pieces[0] + '"test-token"' + pieces[1];
const script = page.match(/<script>([\s\S]*?)<\/script>/)[1];
const elements = new Map();
const el = id => {
  if (!elements.has(id)) elements.set(id, {value: '', checked: false, disabled: false,
    textContent: '', addEventListener() {}, reportValidity() { return true; }});
  return elements.get(id);
};
const buttons = ['stop', 'manual', 'solar', 'legacy'].map(mode => ({dataset: {mode}}));
const state = {enabled: true, online: true, measured: true, amps: 10.5, power_w: 2415,
  mode: 'legacy', mode_label: 'Main rendue a la borne', target_a: -1,
  manual_limit_a: 32, manual_allowed: true, soc_api: true,
  soc_guard: true, soc_stop: 30, soc_resume: 35, result: ''};
const calls = [];
const context = {document: {getElementById: el, querySelectorAll: () => buttons},
  URLSearchParams, setInterval() {}, fetch: async (url, options) => {
    if (!options?.method) return {ok: true, json: async () => state};
    calls.push({url, options});
    return {ok: true, text: async () => 'Commande en attente'};
  }};
vm.createContext(context);
vm.runInContext(script, context); // Parses and executes the exact production UI script.
const settle = () => new Promise(resolve => setImmediate(resolve));
(async () => {
  await settle();
  assert.equal(el('mode').textContent, 'Main rendue a la borne');
  assert.match(el('measure').textContent, /~2415 W/);
  assert.equal(el('target').textContent, 'Main rendue à la borne');
  assert.equal(calls.length, 0); // Opening and refreshing never sends a command.
  buttons[3].onclick();
  await settle();
  assert.equal(calls[0].url, '/vetronic/mode');
  assert.equal(calls[0].options.headers['X-CSRF-Token'], 'test-token');
  assert.equal(calls[0].options.body.get('mode'), 'legacy');
  state.manual_allowed = false;
  await vm.runInContext('status()', context);
  assert.equal(buttons[1].disabled, true);
  assert.equal(buttons[3].disabled, false); // Tariffs cannot prevent handing control back.
  state.busy = true;
  await vm.runInContext('status()', context);
  for (const b of buttons) assert.equal(b.disabled, true);
  state.busy = false; state.online = false; state.measured = false;
  await vm.runInContext('status()', context);
  assert.match(el('measure').textContent, /-- A/);
  assert.equal(buttons[3].disabled, true);
  state.online = true; state.soc_api = true;
  await vm.runInContext('status()', context);
  el('socStop').value = 30; el('socResume').value = 34;
  el('saveSoc').onclick(); await settle();
  assert.equal(calls.length, 1);
  assert.match(el('result').textContent, /5 points/);
  el('socResume').value = 35;
  el('saveSoc').onclick(); await settle();
  assert.equal(calls[1].url, '/vetronic/soc');
  assert.equal(calls[1].options.body.get('stop'), '30');
  assert.equal(calls[1].options.body.get('resume'), '35');
  console.log('PASS: production Web script, return control, CSRF, no startup POST, measured/offline state, tariffs and SOC drafts');
})().catch(error => { console.error(error); process.exitCode = 1; });
