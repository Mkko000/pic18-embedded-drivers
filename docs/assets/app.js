(function(){
  const C=window.COURSE, all=C.parts.flatMap(p=>p.lessons), KEY='pic-done';
  const side=document.getElementById('side'), main=document.getElementById('main');
  const done=()=>{try{return JSON.parse(localStorage.getItem(KEY)||'[]')}catch(e){return []}};
  const setDone=a=>{localStorage.setItem(KEY,JSON.stringify(a));render();};
  function render(){
    const d=done(); let n=0, cur=(location.hash.match(/#\/(.+)/)||[])[1];
    side.innerHTML=C.parts.map(p=>`<h4>${p.title}</h4>`+p.lessons.map(l=>{n++;return `<a href="#/${l.id}" class="${d.includes(l.id)?'done':''} ${cur===l.id?'on':''}"><span class="n">${d.includes(l.id)?'✓':n}</span>${l.title}</a>`}).join('')).join('');
    const pct=Math.round(100*all.filter(l=>d.includes(l.id)).length/all.length);
    document.getElementById('progBar').style.width=pct+'%';
    document.getElementById('progTxt').textContent=pct+'%';
  }
  function home(){
    const d=done(), next=all.find(l=>!d.includes(l.id))||all[0];
    main.innerHTML=`<section class="hero">
      <p class="meta">كورس تفاعلي · PIC18F4620 · MPLAB XC8</p>
      <h1>نتعلم Embedded على PIC18، من أول LED لحد ما نكتب درايفرات بجد</h1>
      <p>الكورس ده ماشي جنب مشروع <a href="https://github.com/Mkko000/pic18-embedded-drivers">pic18-embedded-drivers</a> خطوة بخطوة. كل حاجة اتكتبت في الكود ليها هنا شرح: ليه اتكتبت كده، وإيه اللي كان ممكن يبوظ لو اتكتبت بطريقة تانية. فيه حاسبات تجرّب فيها بأرقامك، وأسئلة صغيرة بعد كل جزء، والصفحة بتفتكر إنت وصلت لحد فين.</p>
      <p>الدروس هتزيد مع المشروع. كل ما ندخل درايفر جديد (زرار، ADC، تايمرز، UART) هيبقى ليه درس هنا.</p>
      <a class="btn main" href="#/${next.id}">${d.length?'كمّل من':'ابدأ بـ'}: ${next.title}</a>
      <div class="cards">${all.map((l,i)=>`<a href="#/${l.id}" class="${d.includes(l.id)?'done':''}"><small>درس ${i+1} · ${l.min} دقيقة${d.includes(l.id)?' · خلصته':''}</small><br><b>${l.title}</b></a>`).join('')}</div>
      <footer>محمد السيسي · ${new Date().getFullYear()}</footer></section>`;
  }
  async function lesson(id){
    const i=all.findIndex(l=>l.id===id); if(i<0) return home();
    const l=all[i]; main.innerHTML='<article><p class="meta">بيحمّل…</p></article>';
    let html; try{const r=await fetch('lessons/'+l.file); if(!r.ok) throw 0; html=await r.text();}catch(e){html='<p>الدرس ماحمّلش. جرّب تعمل refresh.</p>'}
    const prev=all[i-1], next=all[i+1], isDone=done().includes(id);
    main.innerHTML=`<article><p class="meta">درس ${i+1} من ${all.length} · حوالي ${l.min} دقيقة</p><h1>${l.title}</h1>${html}
      <div class="done-box">${prev?`<a class="btn" href="#/${prev.id}">→ ${prev.title}</a>`:'<span></span>'}
      <button class="btn ${isDone?'':'main'}" id="doneBtn">${isDone?'✓ خلصته (دوس عشان تلغي)':'خلصت الدرس ده'}</button>
      ${next?`<a class="btn" href="#/${next.id}">${next.title} ←</a>`:'<span></span>'}</div></article>`;
    document.getElementById('doneBtn').onclick=()=>{let d=done(); d=d.includes(id)?d.filter(x=>x!==id):d.concat(id); setDone(d); if(!isDone&&next) location.hash='#/'+next.id; else lesson(id);};
    enhance(main); main.focus({preventScroll:true}); window.scrollTo(0,0);
  }
  function hl(s){return s.replace(/(\/\*[\s\S]*?\*\/|\/\/[^\n]*)|("[^"\n]*")|(#(?:include|define|ifdef|ifndef|if|else|endif|error|pragma)\b|\b(?:typedef|struct|enum|if|else|return|const|static|void|for|while|break|switch|case|volatile|#include|#define|#if|#ifdef|#else|#endif|#error|#pragma|uint8|uint16|Std_ReturnType|NULL)\b)|\b(0x[0-9A-Fa-f]+|\d+U?L?)\b/g,(m,c,s2,k,n)=>c?`<span class="c">${c}</span>`:s2?`<span class="s">${s2}</span>`:k?`<span class="k">${k}</span>`:`<span class="m">${n}</span>`)}
  function enhance(root){
    root.querySelectorAll('pre').forEach(p=>{const c=p.querySelector('code'); if(c&&!p.dataset.raw){p.dataset.raw=c.textContent;c.innerHTML=hl(c.innerHTML);}
      const b=document.createElement('button');b.className='cp';b.textContent='نسخ';b.onclick=()=>{navigator.clipboard.writeText(p.dataset.raw||p.textContent).then(()=>{b.textContent='اتنسخ';setTimeout(()=>b.textContent='نسخ',1400)})};p.appendChild(b);});
    root.querySelectorAll('a[data-src]').forEach(a=>{a.href=C.repo+a.dataset.src;a.target='_blank';a.rel='noopener';a.classList.add('src');if(!a.textContent)a.textContent='↗ '+a.dataset.src;});
    root.querySelectorAll('.quiz').forEach(q=>q.querySelectorAll('button').forEach(b=>b.onclick=()=>{if(q.classList.contains('answered'))return;q.classList.add('answered');b.classList.add('ok' in b.dataset?'right':'wrong');q.querySelector('[data-ok]').classList.add('right');}));
    root.querySelectorAll('.w[data-w]').forEach(w=>window.WIDGETS[w.dataset.w]&&window.WIDGETS[w.dataset.w](w));
  }
  document.getElementById('themeBtn').onclick=()=>{const d=document.documentElement;const t=d.dataset.theme==='dark'?'':'dark';d.dataset.theme=t;localStorage.setItem('pic-theme',t||'light')};
  document.getElementById('menuBtn').onclick=()=>side.classList.toggle('open');
  side.addEventListener('click',e=>{if(e.target.closest('a'))side.classList.remove('open')});
  function route(){render();const id=(location.hash.match(/#\/(.+)/)||[])[1];id?lesson(id):home();}
  window.addEventListener('hashchange',route); route();
})();
