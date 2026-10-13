/* الأدوات التفاعلية. كل أداة بتتنادي من الدرس بـ <div class="w" data-w="اسمها"></div> */
(function(){
  const $=(w,s)=>w.querySelector(s), num=v=>parseFloat(String(v).replace(',','.'));
  const E12=[1,1.2,1.5,1.8,2.2,2.7,3.3,3.9,4.7,5.6,6.8,8.2];
  function nextStd(r){if(!(r>0))return NaN;const d=Math.pow(10,Math.floor(Math.log10(r)));for(const e of E12){if(e*d>=r-1e-9)return +(e*d).toPrecision(3)}return 10*d}
  const fmtR=r=>r>=1000?(r/1000).toFixed(r>=10000?1:2).replace(/\.?0+$/,'')+' kΩ':Math.round(r)+' Ω';
  const fmtI=a=>a>=1?a.toFixed(2)+' A':(a*1000).toFixed(a<0.01?2:1)+' mA';
  function bind(w,fn){w.querySelectorAll('input,select').forEach(i=>i.oninput=fn);fn();}
  const W={};

  W['bits']=w=>{ // TRIS + LAT لـ PORTC مع ليدات
    w.innerHTML=`<h5>جرّب بنفسك: TRISC و LATC</h5><p style="margin:0 0 8px;font-size:15px">دوس على أي بت تقلبه. الليد تنوّر بس لو الرجل <b>Output</b> (TRIS=0) <b>و</b> الـ LAT بتاعها 1.</p>
    <div style="font-size:14px;color:var(--mute)">TRISC</div><div class="bits" id="t"></div>
    <div style="font-size:14px;color:var(--mute);margin-top:8px">LATC</div><div class="bits" id="l"></div>
    <div class="leds" id="leds"></div><div class="out" id="o"></div>`;
    let tris=0xFF, lat=0x00;
    const mk=(el,get,set)=>{el.innerHTML='';for(let b=7;b>=0;b--){const d=document.createElement('div');d.className='bit';const bt=document.createElement('button');bt.textContent=(get()>>b)&1;if((get()>>b)&1)bt.className='one';bt.onclick=()=>{set(get()^(1<<b));draw()};d.append(bt,'RC'+b);el.append(d)}};
    const hex=v=>'0x'+v.toString(16).toUpperCase().padStart(2,'0');
    function draw(){mk($(w,'#t'),()=>tris,v=>tris=v);mk($(w,'#l'),()=>lat,v=>lat=v);
      $(w,'#leds').innerHTML=[7,6,5,4,3,2,1,0].map(b=>`<div class="led ${(!((tris>>b)&1)&&((lat>>b)&1))?'on':''}" title="RC${b}"></div>`).join('');
      const floating=[...Array(8).keys()].filter(b=>((tris>>b)&1)&&((lat>>b)&1));
      $(w,'#o').innerHTML=`<code>TRISC = ${hex(tris)};</code> &nbsp; <code>LATC = ${hex(lat)};</code>`+(floating.length?`<br>RC${floating.join(', RC')}: الـ LAT فيها 1 بس الرجل لسه Input، فالـ 1 ده مستني ومش طالع برّه. أول ما تخليها Output هتنوّر على طول.`:'')}
    draw();
  };

  W['led-r']=w=>{
    w.innerHTML=`<h5>حاسبة مقاومة الليد</h5>
    <label>جهد المصدر (V)<input id="vs" value="5"></label>
    <label>لون الليد<select id="c"><option value="2.0">أحمر 2.0V</option><option value="2.1">أصفر 2.1V</option><option value="3.2">أخضر 3.2V</option><option value="3.2">أزرق 3.2V</option><option value="3.3">أبيض 3.3V</option></select></label>
    <label>Vf (V)<input id="vf" value="2.0"></label>
    <label>التيار (mA)<input id="i" value="10"></label><div class="out" id="o"></div>`;
    $(w,'#c').onchange=()=>{$(w,'#vf').value=$(w,'#c').value;calc()};
    function calc(){const vs=num($(w,'#vs').value),vf=num($(w,'#vf').value),i=num($(w,'#i').value)/1000,o=$(w,'#o');
      if(!(i>0)||vs<=vf){o.className='out bad';o.innerHTML=vs<=vf?'الجهد اللي فاضل للمقاومة صفر أو سالب. المصدر ده مش هيقدر ينوّر الليد دي.':'اكتب تيار أكبر من صفر.';return}
      const r=(vs-vf)/i,std=nextStd(r),ireal=(vs-vf)/std,p=ireal*ireal*std;
      o.className='out'+(ireal>0.025?' bad':' good');
      o.innerHTML=`R المحسوبة = (${vs} − ${vf}) ÷ ${(i*1000).toFixed(1)}mA = <b>${fmtR(r)}</b><br>أقرب قيمة في السوق أكبر منها: <b>${fmtR(std)}</b> ← التيار الفعلي ${fmtI(ireal)}، والمقاومة هتسخن بـ ${(p*1000).toFixed(1)} mW (ربع وات تكفي وزيادة)`+(ireal>0.025?'<br>التيار ده أكبر من 25mA، الرجل مش هتستحمل. كبّر المقاومة.':'')}
    bind(w,calc);
  };

  W['port-current']=w=>{
    w.innerHTML=`<h5>هل الشريحة هتستحمل؟</h5>
    <label>عدد الليدات المنوّرة مع بعض<input id="n" value="8"></label>
    <label>تيار الليد الواحدة (mA)<input id="i" value="10"></label>
    <label>استهلاك الـ MCU نفسه (mA)<input id="m" value="2.5"></label><div class="out" id="o"></div>`;
    bind(w,()=>{const n=num($(w,'#n').value),i=num($(w,'#i').value),m=num($(w,'#m').value),t=m+n*i,o=$(w,'#o');
      const pinBad=i>25,portBad=n*i>200;o.className='out '+(pinBad||portBad?'bad':'good');
      o.innerHTML=`الإجمالي = ${m} + ${n} × ${i} = <b>${t.toFixed(1)} mA</b><br>`+(pinBad?'كل رجل بتعدّي 25mA. هتتحرق قبل ما الإجمالي يفرق.':portBad?`الرجول لوحدها ${n*i}mA، يعني فوق حد الـ 200mA للبورتات كلها. محتاج ترانزستورات أو تقلل التيار.`:`في الأمان. فاضلك ${(200-n*i).toFixed(0)}mA قبل حد البورتات.`)});
  };

  W['base-r']=w=>{
    w.innerHTML=`<h5>حاسبة مقاومة القاعدة (BJT كمفتاح)</h5>
    <label>تيار الحمل Ic (mA)<input id="ic" value="100"></label>
    <label>hFE (من الداتاشيت، أقل قيمة)<input id="h" value="100"></label>
    <label>معامل الأمان (overdrive)<input id="k" value="2"></label>
    <label>جهد الرجل (V)<input id="v" value="5"></label><div class="out" id="o"></div>`;
    bind(w,()=>{const ic=num($(w,'#ic').value)/1000,h=num($(w,'#h').value),k=num($(w,'#k').value),v=num($(w,'#v').value),o=$(w,'#o');
      const ib=ic/h*k,rb=(v-0.7)/ib,std=[1e3,1.5e3,2.2e3,3.3e3,4.7e3,6.8e3,10e3,15e3,22e3,33e3,47e3].reverse().find(x=>x<=rb)||1000,ibr=(v-0.7)/std;
      o.className='out '+(ibr>0.025?'bad':'good');
      o.innerHTML=`Ib المطلوب = ${(ic*1000).toFixed(0)}mA ÷ ${h} × ${k} = <b>${fmtI(ib)}</b><br>Rb = (${v} − 0.7) ÷ Ib = ${fmtR(rb)} ← خد القيمة الأصغر اللي بعدها: <b>${fmtR(std)}</b> (Ib الفعلي ${fmtI(ibr)})`+(ibr>0.025?'<br>القاعدة عايزة أكتر من 25mA من الرجل. هنا لازم MOSFET أو دارلنجتون.':'')});
  };

  W['fmax']=w=>{
    w.innerHTML=`<h5>أقصى تردد على جهد معيّن (PIC18LF4620)</h5>
    <label>VDD (V)<input id="v" type="range" min="2" max="5.5" step="0.1" value="3.3" style="width:220px"></label><div class="out" id="o"></div>`;
    bind(w,()=>{const v=num($(w,'#v').value),f=16.36*(v-2)+4,real=Math.min(f,40),o=$(w,'#o');o.className='out';
      o.innerHTML=`VDD = <b>${v.toFixed(1)}V</b><br>Fmax = 16.36 × (${v.toFixed(1)} − 2) + 4 = ${f.toFixed(1)} MHz`+(f>40?` ← بس الشريحة نفسها سقفها 40 MHz، فالمسموح <b>40 MHz</b>`:` ← متعدّيش <b>${real.toFixed(1)} MHz</b>`)+(v<4.2?'<br>وخلي بالك: تحت 4.2V محتاج نسخة الـ LF، الـ PIC18F4620 العادية مش مضمونة هنا.':'')});
  };

  W['battery']=w=>{
    w.innerHTML=`<h5>البطارية هتقعد قد إيه؟</h5>
    <label>سعة البطارية (mAh)<input id="c" value="1000"></label>
    <label>تيار الصحيان (mA)<input id="a" value="2.5"></label>
    <label>تيار النوم (µA)<input id="s" value="0.2"></label>
    <label>نسبة الوقت صاحي (%)<input id="d" value="1"></label><div class="out" id="o"></div>`;
    bind(w,()=>{const c=num($(w,'#c').value),a=num($(w,'#a').value),s=num($(w,'#s').value)/1000,d=num($(w,'#d').value)/100,o=$(w,'#o');
      const avg=a*d+s*(1-d),h=c/avg,always=c/a;const nice=h=>h<48?h.toFixed(0)+' ساعة':h<24*60?(h/24).toFixed(0)+' يوم':(h/24/30).toFixed(1)+' شهر';o.className='out good';
      o.innerHTML=`متوسط الاستهلاك = <b>${avg<1?(avg*1000).toFixed(1)+' µA':avg.toFixed(2)+' mA'}</b><br>العمر التقريبي: <b>${nice(h)}</b> بدل ${nice(always)} لو فضل صاحي على طول.<br><small>ده حساب نظري. البطارية في الحقيقة بتفقد شوية من نفسها، والـ BOR والـ WDT لو شغالين بيزودوا تيار النوم.</small>`});
  };

  W['divider']=w=>{
    w.innerHTML=`<h5>مقسّم الجهد على القاعدة</h5>
    <label>R3 من الرجل (kΩ)<input id="a" value="4.7"></label><label>R4 للأرضي (kΩ)<input id="b" value="10"></label><label>جهد الرجل (V)<input id="v" value="5"></label><div class="out" id="o"></div>`;
    bind(w,()=>{const a=num($(w,'#a').value),b=num($(w,'#b').value),v=num($(w,'#v').value),vb=v*b/(a+b),o=$(w,'#o');o.className='out '+(vb>=0.7?'good':'bad');
      o.innerHTML=`من غير ترانزستور: V<sub>B</sub> = ${v} × ${b} ÷ (${a} + ${b}) = <b>${vb.toFixed(2)}V</b><br>`+(vb>=0.7?'كفاية تفتح القاعدة. ولما الرجل تبقى LOW أو Input، R4 بتشد القاعدة للأرضي فالترانزستور يقفل أكيد بدل ما يفضل سايب.':'أقل من 0.7V، الترانزستور مش هيفتح. صغّر R3 أو كبّر R4.')});
  };

  W['source-sink']=w=>{
    w.innerHTML=`<h5>نفس الليد، توصيلتين</h5>
    <label>طريقة التوصيل<select id="m"><option value="src">Source: الرجل ← مقاومة ← ليد ← GND</option><option value="snk">Sink: VCC ← مقاومة ← ليد ← الرجل</option></select></label>
    <label>اللي مكتوب على الرجل<select id="l"><option value="1">GPIO_HIGH</option><option value="0">GPIO_LOW</option></select></label>
    <div class="leds"><div class="led" id="d"></div></div><div class="out" id="o"></div>`;
    bind(w,()=>{const src=$(w,'#m').value==='src',hi=$(w,'#l').value==='1',on=src?hi:!hi;$(w,'#d').className='led'+(on?' on':'');
      $(w,'#o').innerHTML=on?(src?'التيار طالع من الرجل للأرضي. الشريحة هي اللي بتدّي (Source).':'التيار جاي من الـ VCC وداخل في الرجل. الشريحة هي اللي بتشرب (Sink).'):(src?'الرجل 0V والليد طرفها التاني على الأرضي، مفيش فرق جهد، مفيش نور.':'الرجل 5V والطرف التاني 5V، مفيش فرق جهد، مفيش نور.');});
  };

  window.WIDGETS=W;
})();
