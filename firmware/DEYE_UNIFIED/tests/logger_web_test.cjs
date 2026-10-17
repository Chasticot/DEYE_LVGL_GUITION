const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync(path.join(__dirname, '../web_logger_discovery.h'), 'utf8');
const script = source.match(/<script>([\s\S]*?)<\/script>/)[1];
const fields = {host: {value: 'old-host'}, serial: {value: '2712345678'}, port: {value: '8899'},unit:{value:'1'}};
const transport = {value: 'lsw', addEventListener(event, cb) { this.change = cb; }};
const candidates={children:[],replaceChildren(){this.children=[];},appendChild(child){this.children.push(child);}};
const status = {textContent: ''};
const button = {addEventListener(event, cb) { this.click = cb; }};
const form = {querySelector(selector) { return fields[selector.match(/name=(\w+)/)[1]]; }};
const calls = [];
let state = {state: 0, generation: 0};
let failure = false, startFailure = false, active = false;
const context = {
  token: 'test-csrf', URLSearchParams,
  document: {querySelector() { return form; },getElementById(id) { return id === 'logger-status' ? status : id === 'logger-transport' ? transport : id === 'logger-candidates' ? candidates : button; },
    createElement(){return {addEventListener(event,cb){this.click=cb;}};}},
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
  state = {state: 3, generation: 1, host: '192.168.1.12', serial: 2712345678,identity:'2712345678',model:'lsw',port:8899,unit:1};
  await vm.runInContext('pollLogger()', context);
  assert(!active);
  assert.equal(fields.host.value, '192.168.1.12');
  assert.equal(fields.serial.value, '2712345678');
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
  failure=false;startFailure=false;
  transport.value='lse';transport.change();
  assert.equal(fields.port.value,8899);assert(!button.disabled);
  fields.serial.value='';let before=calls.length;await button.click();assert.equal(calls.length,before);
  fields.serial.value='123456789';await button.click();assert.equal(calls.length,before);
  fields.serial.value='2101769527';
  state={state:2,phase:4,scanned:10,total:510};await button.click();
  assert.match(status.textContent,/Modbus TCP/);
  const lseStart=calls.filter(c=>c.url.endsWith('/start')).at(-1);
  assert.equal(lseStart.options.body.get('model'),'lse');assert.equal(lseStart.options.body.get('port'),'8899');
  state={state:6,generation:3,model:'lse',identity:'2101769527',port:8899,unit:1,candidates:['192.168.1.47','192.168.1.48']};
  fields.host.value='user-edit';await vm.runInContext('pollLogger()',context);
  assert.equal(fields.host.value,'user-edit');assert.equal(candidates.children.length,2);
  assert.match(status.textContent,/non verifie/);candidates.children[0].click();
  assert.equal(fields.host.value,'192.168.1.47');assert.equal(fields.serial.value,'2101769527');
  const staleButton=candidates.children[1];fields.serial.value='999';staleButton.click();assert.equal(fields.host.value,'192.168.1.47');
  await vm.runInContext('pollLogger()',context);assert.equal(candidates.children.length,0);assert.match(status.textContent,/autre type/);
  transport.value='dl1000b';transport.change();assert.equal(fields.port.value,502);assert(!button.disabled);
  fields.serial.value='d123456789012';state={state:1};await button.click();
  const dlStart=calls.filter(c=>c.url.endsWith('/start')).at(-1);assert.equal(dlStart.options.body.get('serial'),'D123456789012');
  state={state:3,generation:4,host:'192.168.1.50',model:'dl1000b',identity:'D123456789012',port:502,unit:1};
  await vm.runInContext('pollLogger()',context);assert.equal(fields.host.value,'192.168.1.50');
  fields.port.value=1502;await vm.runInContext('pollLogger()',context);assert.match(status.textContent,/autre type/);
  await vm.runInContext('updateLoggerTransport(false)',context);assert.equal(fields.port.value,1502);
  transport.value = 'lsw'; transport.change();
  assert.equal(fields.port.value, 8899); assert(!fields.serial.disabled && !button.disabled);
  console.log('Production logger Web tests passed.');
}
run().catch(error => { console.error(error); process.exitCode = 1; });
