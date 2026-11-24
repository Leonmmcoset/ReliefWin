> ReliefWin has no public support endpoint configured. Contact the maintainer of this checkout.

<div dir="rtl" lang="ar">

# تثبيت ReliefWin

> [English](../../INSTALL.md) | [中文](../zh-CN/INSTALL.md) | [Français](../fr-FR/INSTALL.md) | [Español](../es-ES/INSTALL.md) | [日本語](../ja-JP/INSTALL.md) | **العربية** | [Українська](../uk-UA/INSTALL.md)

## بناء الصور

جهّز بيئة البناء واضبط عملية بناء كما هو موضّح في [بناء ReliefWin بنفسك](README.md#بناء-reliefwin-بنفسك). ثم ابنِ الصورة التي تحتاجها من مجلد الإخراج، مثلًا ‎`output-Clang-amd64-debug`‎:

| الأمر | ينشئ | استخدمه من أجل |
|---|---|---|
| ‎`ninja bootcd`‎ | ‎`bootcd.iso`‎ | تشغيل ReliefWin من صورة قرص مضغوط، أو بدء برنامج إعداد ReliefWin منها وتثبيت ReliefWin على قرص. |
| ‎`ninja reliefwinimg`‎ | ‎`ReliefWin.img`‎ | إقلاع قرص مثبّت عليه ReliefWin مسبقًا. يقلع عبر UEFI أو BIOS. |
| ‎`ninja reliefwinvhd`‎ | ‎`ReliefWin.vhd`‎، ‎`ReliefWin.vhdx`‎ | استخدام القرص المثبّت مسبقًا نفسه في الآلات الافتراضية: VHD في VirtualBox وHyper-V من الجيل الأول، وVHDX في Hyper-V من الجيل الثاني الذي لا يقبل أقراص VHD. |

يبني كل هدف أولًا كل ما يحتاجه، وتُكتب الملفات في مجلد الإخراج.

## التثبيت من قرص الإعداد

أقلع ‎`bootcd.iso`‎ في آلة افتراضية أو على حاسوب، واتبع تعليمات برنامج إعداد ReliefWin. يعرض برنامج الإعداد الأقراص كما يفعل برنامج إعداد Windows، ويستطيع إنشاء الأقسام وحذفها وتهيئتها وتقليصها وتوسيعها. ويثبّت ReliefWin على NTFS وينشئ قسم نظام EFI الذي يحتاجه.

## استخدام القرص المثبّت مسبقًا

لا يحتاج ‎`ReliefWin.img`‎ إلى أي إعداد. شغّله في آلة افتراضية مثل QEMU، أو اكتبه على وحدة USB أو بطاقة SD وأقلع الجهاز منها. استخدم ‎`ReliefWin.vhd`‎ للآلات الافتراضية التي تقبل أقراص VHD، و‎`ReliefWin.vhdx`‎ للآلات الافتراضية Hyper-V من الجيل الثاني، مع تعطيل الإقلاع الآمن.

يحمل القرص المثبّت مسبقًا أيضًا برنامج إعداد ReliefWin. شغّل ‎`C:\<المعمارية>\reliefwin.exe`‎، مثلًا ‎`C:\arm64\reliefwin.exe`‎، لتثبيت ReliefWin على قرص آخر من النظام قيد التشغيل.

## المساعدة

إذا حدث خطأ ما، فأبلغ عنه في Issues مع ذكر المعمارية ونوع البناء والإيداع الذي بنيته والخطوات التي تؤدي إلى المشكلة. توجد الأدلة والملاحظات في Wiki، ويمكنك سؤال الفريق على Discord أو Matrix.

</div>
