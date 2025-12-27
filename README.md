# Highway Racer 🏎️💨

C++ ve SFML kütüphanesi kullanılarak geliştirilmiş, Nesne Yönelimli Programlama (OOP) prensiplerine dayalı, dinamik ve akıcı bir 2D araba yarışı oyunu.

## 🚀 Proje Hakkında
Bu proje, başlangıç seviyesindeki bir kod yapısının; modern yazılım mimarisi, bellek yönetimi ve performans optimizasyonları ile nasıl profesyonel bir hale getirilebileceğini göstermektedir.

## ✨ Öne Çıkan Özellikler
- **Gelişmiş Mimari:** Kod yapısı; Logic (Oyun Mantığı), Rendering (Çizim) ve Asset Management (Varlık Yönetimi) katmanlarına ayrılarak modüler hale getirildi.
- **Dinamik Trafik Yapay Zekası:** Diğer araçlar şerit değiştirme, sollama ve hızlanma kararları verebilen bir mantığa sahiptir.
- **Gece Modu:** 'N' tuşu ile aktif edilebilen, özel headlight efektleri ve karartılmış görseller içeren gece sürüş modu.
- **Performans Optimizasyonu:** Ağır nesneler `const reference` ile aktarılarak gereksiz kopyalamalar önlendi; manuel bellek yönetimi (destructors) ile bellek sızıntıları (memory leaks) minimize edildi.

## 🛠️ Teknik Detaylar
- **Dil:** C++
- **Kütüphane:** SFML (Simple and Fast Multimedia Library)
- **Desenler:** Singleton benzeri Asset Manager, OOP Sınıf Yapıları (Game, Player, Car).
- **Klasör Yapısı:** - `src/`: Kaynak kodlar (.cpp)
  - `include/`: Başlık dosyaları (.h)
  - `assets/`: Görsel, ses ve font dosyaları.

## 🎮 Kurulum ve Çalıştırma
1. Bu depoyu klonlayın: `git clone https://github.com/kullanici-adin/Highway-Racer.git`
2. Sisteminizde **SFML 2.6.x** kurulu olduğundan emin olun.
3. `Highway-Racer.sln` dosyasını Visual Studio ile açın.
4. Proje özelliklerinden SFML `include` ve `lib` yollarını kendi sisteminize göre güncelleyin.
5. `Release` modunda derleyin ve çalıştırın.

## 🕹️ Kontroller
- **W/A/S/D:** Aracı kontrol eder (Sol/Geri/Sağ/İleri).
- **N:** Gece / Gündüz modu geçişi.
- **ESC:** Oyunu duraklatır (Pause Menüsü).
- **F11:** Tam ekran modu aç/kapat.
- **Oyundan Çıkış:** Tam ekran modundan (F11) çıktıktan sonra pencerenin sağ üstündeki kapatma (X) butonu ile güvenli çıkış yapılır.

## 📈 Gelişim Süreci (Evolution)
Bu proje, tek bir dosya içerisindeki spagetti kod yapısından, profesyonel ve modüler bir yazılım mimarisine dönüştürülmüştür.

- **Güncel Versiyon:** Şu an bulunduğunuz bu depo.
- **Eski Versiyon (Arşiv):** [Highway-Racer-CPP (Legacy)](https://github.com/ardablmezz/Highway-Racer-CPP)

**Neler Değişti?**
- Fonksiyonel yapıdan **Nesne Yönelimli Programlamaya (OOP)** geçildi.
- Klasör yapısı `src` ve `include` olarak profesyonel standartlara göre ayrıldı.
- Bellek yönetimi ve varlık (asset) yönetimi optimize edildi.

---
