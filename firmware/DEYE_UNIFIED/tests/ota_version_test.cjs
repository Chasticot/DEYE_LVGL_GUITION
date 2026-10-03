const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');

// Execute the real C++-embedded JavaScript, substituting the compiled version.
const source = fs.readFileSync(path.join(__dirname, '../web_server.h'), 'utf8');
const line = source.split(/\r?\n/).find(s => s.includes('function semver('));
assert(line, 'Production OTA version script missing');
const strings = [...line.matchAll(/"((?:\\.|[^"\\])*)"/g)].map(m => JSON.parse('"' + m[1] + '"'));
assert.equal(strings.length, 2);

async function check(installed, response, expected, status = 200) {
  const result = {textContent: ''};
  const calls = [];
  const context = {
    document: {getElementById: () => result, querySelectorAll: () => []},
    fetch: async (url, options) => {
      calls.push({url, options});
      if (response instanceof Error) throw response;
      return {ok: status >= 200 && status < 300, status, json: async () => response};
    },
  };
  vm.createContext(context);
  vm.runInContext("const token='test-token" + strings[0] + installed + strings[1].split('</script>')[0], context);
  await vm.runInContext('checkRelease()', context);
  assert.match(result.textContent, expected, installed + ' / ' + JSON.stringify(response));
  assert.equal(calls.length, 1);
  assert.equal(calls[0].url, 'https://api.github.com/repos/Chasticot/DEYE_LVGL_GUITION/releases/latest');
  assert(!calls[0].options.method, 'Checking a release must not send POST or install firmware');
}

(async () => {
  await check('4.3.4-vetronic-v3', {tag_name: 'v4.3.7'}, /plus recente.*v4\.3\.7/);
  await check('v4.3.4-vetronic-v3', {tag_name: 'v4.3.7'}, /plus recente/);
  await check('4.3.7', {tag_name: 'v4.3.7'}, /firmware est a jour/);
  await check('4.3.7', {tag_name: 'v4.3.6'}, /firmware est a jour/);
  await check('4.3.9', {tag_name: 'v4.3.10'}, /plus recente/);
  await check('4.3.10', {tag_name: 'v4.3.9'}, /firmware est a jour/);
  await check('4.3.4-vetronic-v3+build.2', {tag_name: 'v4.3.7'}, /plus recente/);
  await check('inconnue', {tag_name: 'v4.3.7'}, /Verification impossible.*version installee/i);
  await check('4.3.7', {tag_name: 'inconnu'}, /Verification impossible.*tag/i);
  await check('4.3.7', null, /Verification impossible.*Reponse GitHub/i);
  await check('4.3.7', {}, /Verification impossible.*Reponse GitHub/i);
  await check('4.3.7', {}, /Aucune release/, 404);
  await check('4.3.7', {}, /erreur 403/, 403);
  await check('4.3.7', new Error('Failed to fetch'), /Verification impossible.*Failed to fetch/);
  console.log('PASS: production OTA release check, legacy suffixes, numeric comparison, invalid versions/API and GET only');
})().catch(error => { console.error(error); process.exitCode = 1; });
