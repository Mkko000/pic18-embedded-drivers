/* قايمة الدروس. عشان تضيف درس جديد:
   1) اعمل ملف في docs/lessons/ (اتبع شكل أي درس موجود)
   2) ضيف سطر هنا بنفس الترتيب اللي عايزه يظهر بيه */
window.COURSE = {
  repo: 'https://github.com/Mkko000/pic18-embedded-drivers/blob/main/',
  parts: [
    { title: 'الأساس', lessons: [
      { id: 'start',      file: '01-start.html',      title: 'قبل ما نبدأ: البورد والريجسترات', min: 15 },
      { id: 'source-sink',file: '02-source-sink.html',title: 'Source و Sink: مين بيدّي التيار؟', min: 15 },
      { id: 'transistor', file: '03-transistor.html', title: 'الترانزستور كمفتاح', min: 25 },
      { id: 'power',      file: '04-power.html',      title: 'الكهربا والطاقة من الداتاشيت', min: 25 }
    ]},
    { title: 'جوّه المشروع', lessons: [
      { id: 'layers',     file: '05-layers.html',     title: 'طبقات المشروع: Config و Common', min: 20 },
      { id: 'gpio',       file: '06-gpio.html',       title: 'درايفر الـ GPIO: الـ Direct والـ Struct', min: 25 },
      { id: 'led',        file: '07-led.html',        title: 'درايفر الـ LED و led_initialize', min: 20 },
      { id: 'projects',   file: '08-projects.html',   title: 'المشاريع الأربعة', min: 20 }
    ]}
  ]
};
