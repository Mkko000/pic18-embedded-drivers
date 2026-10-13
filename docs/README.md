# كورس PIC18 (الموقع)

ده الموقع بتاع الكورس، بيتنشر على GitHub Pages من فولدر `docs/`.

## إضافة درس جديد
1. اعمل ملف في `lessons/` (مثلًا `09-button.html`). الملف HTML عادي من غير `<html>` ولا عنوان؛ العنوان بيتحط من القايمة.
2. ضيف سطر في `assets/lessons.js` في الجزء المناسب: `{ id, file, title, min }`.

## أدوات تقدر تستخدمها جوّه الدرس
- `<div class="w" data-w="led-r"></div>` حاسبة (الأسماء في `assets/widgets.js`: bits, led-r, port-current, base-r, divider, fmax, battery, source-sink).
- `<aside class="tip"><b>خلي بالك</b>...</aside>`
- `<div class="quiz"><p class="q">...</p><button data-ok>الصح</button><button>غلط</button><p class="why">ليه</p></div>`
- `<a data-src="MCAL_Layer/GPIO/hal_gpio.c"></a>` لينك للملف في الريبو.
- `<pre><code>...</code></pre>` كود مع زرار نسخ.
- `<pre class="diff"><code>` كود فيه سطور بتبدأ بـ `+` (إضافة) و `-` (حذف) بيتلوّن، وزرار النسخ بينسخ النسخة الجديدة بس.
- `<div class="goal"><b>الهدف:</b> ...</div>` و `<div class="oops"><b>غلطات مشهورة</b>...</div>`
- تمرين بحل مستخبي: `<details class="ex"><summary>العنوان</summary><p class="task">...</p><details><summary>اعرض الحل</summary>...</details></details>`
