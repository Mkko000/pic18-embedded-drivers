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
    ]},
    { title: 'نكتب الدرايفر بإيدينا', lessons: [
      { id: 'b-setup',    file: '09-b-setup.html',    title: 'من فولدر فاضي: MPLAB X و XC8 وأول ليد', min: 25 },
      { id: 'b-fuses',    file: '10-b-fuses.html',    title: 'الفيوزات وملف الإعدادات', min: 20 },
      { id: 'b-datasheet',file: '11-b-datasheet.html',title: 'نقرا فصل الـ I/O Ports في الداتاشيت', min: 25 },
      { id: 'b-common',   file: '12-b-common.html',   title: 'نبني الطبقات و Common', min: 25 },
      { id: 'b-api',      file: '13-b-api.html',      title: 'نصمم الـ API قبل ما نكتب سطر', min: 25 },
      { id: 'b-direct',   file: '14-b-direct.html',   title: 'الـ Direct API نسخة ورا نسخة', min: 40 },
      { id: 'b-port',     file: '15-b-port.html',     title: 'دوال البورت و PORTE والماسك', min: 25 },
      { id: 'b-struct',   file: '16-b-struct.html',   title: 'الـ Struct API والمفاتيح', min: 25 },
      { id: 'b-analog',   file: '17-b-analog.html',   title: 'Analog ولا Digital: نكتب الجزء الصعب', min: 30 },
      { id: 'b-led',      file: '18-b-led.html',      title: 'درايفر الـ LED من الصفر', min: 30 },
      { id: 'b-project',  file: '19-b-project.html',  title: 'مشروع بالدرايفرات: عدّاد بزرار', min: 30 },
      { id: 'b-debug',    file: '20-b-debug.html',    title: 'لما حاجة متشتغلش: دليل الديباج', min: 25 },
      { id: 'b-style',    file: '21-b-style.html',    title: 'الستايل و Doxygen', min: 20 },
      { id: 'b-final',    file: '22-b-final.html',    title: 'التمارين النهائية', min: 60 }
    ]}
  ]
};
