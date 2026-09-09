const fs = require('fs');
const assert = require('assert');
const source = fs.readFileSync('web_server.h', 'utf8');
const fragment = source.match(/page \+= F\("(';document\.querySelectorAll[\s\S]*?)"\);/)[1];
const script = "const token='test" + JSON.parse('"' + fragment + '"').split('</script>')[0];
let handler, sent, xhr;
const button = {disabled:false};
const status = {textContent:''};
const progress = {value:0};
const form = {action:'/update', addEventListener:(event, callback)=>handler=callback};
const document = {querySelectorAll:selector=>selector==='form'?[form]:[button], getElementById:id=>id==='status'?status:progress};
class Request { constructor(){xhr=this;this.upload={};} open(method,url){assert.equal(method,'POST');assert.equal(url,'/update');} setRequestHeader(name,value){assert.equal(name,'X-CSRF-Token');assert.equal(value,'test');} send(data){sent=data;} }
class Data {constructor(value){assert.equal(value,form);}}
new Function('document','XMLHttpRequest','FormData',script)(document,Request,Data);
handler({preventDefault(){}});
assert(button.disabled && sent);
xhr.upload.onprogress({lengthComputable:true,loaded:25,total:100});
assert.equal(progress.value,25);
xhr.responseText='Mise a jour refusee'; xhr.onload();
assert.equal(status.textContent,xhr.responseText); assert(!button.disabled);
handler({preventDefault(){}}); xhr.onerror(); assert(!button.disabled);
assert(status.textContent.includes('interrompue'));
console.log('PASS: envoi du formulaire, jeton, progression, reponse et erreur reseau.');
