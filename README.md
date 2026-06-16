# Unreal Engine C++ Screw Rotator Component

## Proje Açıklaması

Bu proje, Unreal Engine 5 üzerinde C++ kullanılarak geliştirilmiş modüler bir vida mekanizmasıdır.

Projenin temel amacı, herhangi bir Actor'a eklenebilen bir `UScrewRotatorComponent` oluşturmaktır. Bu component eklendiği Actor'u bir vida mekanizması gibi davranacak şekilde döndürür ve tur sayısına bağlı olarak seçilen lokal eksende ileri veya geri hareket ettirir.

Sistemin temel mantığı C++ ile geliştirilmiştir. Blueprint kullanımı yalnızca test, gösterim ve event kontrolü amacıyla tercih edilmiştir.

---

## Demo Video

Demo video linki:

```text
https://drive.google.com/file/d/1Zi-WkOJv6bU7EZrIJEHmx74K1OogUmv0/view?usp=share_link
```

---

## Sistemin Çalışma Mantığı

Vida mekanizması `CurrentTurn` değeri üzerinden çalışır.

`CurrentTurn`, vidanın kaç tur döndüğünü temsil eder.

Örneğin:

```text
CurrentTurn = 0.0  -> vida tamamen gevşek
CurrentTurn = 1.0  -> vida 1 tam tur dönmüş
CurrentTurn = 1.5  -> vida 1.5 tur dönmüş
```

Sistemde açı ve ilerleme miktarı şu şekilde hesaplanır:

```text
CurrentAngle = CurrentTurn * 360
CurrentDepth = CurrentTurn * ThreadPitch
```

`RotationSpeed`, vidanın saniyede kaç derece döneceğini belirler.

`ThreadPitch`, vidanın bir tam turda lokal eksende ne kadar ilerleyeceğini belirler.

Örneğin:

```text
CurrentTurn = 1.5
ThreadPitch = 50

CurrentAngle = 1.5 * 360 = 540 derece
CurrentDepth = 1.5 * 50 = 75 birim
```

E tuşuna basıldığında `CurrentTurn` değeri artar. Bu durumda vida sıkma yönünde döner ve seçilen lokal eksende ileri hareket eder.

Q tuşuna basıldığında `CurrentTurn` değeri azalır. Bu durumda vida gevşetme yönünde döner ve aynı lokal eksende geri hareket eder.

Sistem `CurrentTurn` değerini her zaman şu aralıkta tutar:

```text
0 <= CurrentTurn <= MaxTurnCount
```

Bu sayede vida maksimum tur sayısını geçemez ve 0 tur değerinin altına inemez.

---

## Kullanılan Sınıflar

### UScrewRotatorComponent

Projenin ana sınıfıdır.

`UActorComponent` sınıfından türetilmiştir. Herhangi bir Actor'a eklenebilir.

Bu component şu görevlerden sorumludur:

- E ve Q tuş girişlerini kontrol etmek
- Vida tur değerini güncellemek
- Actor'un rotasyonunu hesaplamak
- Actor'u lokal eksende ileri veya geri hareket ettirmek
- Debug bilgilerini ekranda göstermek
- Limitlere ulaşıldığında event yayınlamak
- Yardımcı fonksiyonlar ile vida durumunu kontrol etmeyi sağlamak

### EScrewAxis

Vida hareketinin ve dönüşünün hangi lokal eksende gerçekleşeceğini belirleyen enum yapısıdır.

Desteklenen eksenler:

```text
X
Y
Z
```

Bu enum, Unreal Editor üzerinden değiştirilebilir.

---

## Kullanılan Tuşlar

| Tuş | İşlev |
| --- | --- |
| E | Vidayı sıkma yönünde döndürür |
| Q | Vidayı gevşetme yönünde döndürür |

E tuşu basılı tutulduğunda vida `MaxTurnCount` değerine kadar sıkılır.

Q tuşu basılı tutulduğunda vida 0 tur değerine kadar gevşetilir.

---

## Editör Parametrelerinin Açıklamaları

Aşağıdaki parametreler Unreal Editor üzerinden değiştirilebilir.

| Parametre | Açıklama |
| --- | --- |
| RotationSpeed | Vidanın saniyede kaç derece döneceğini belirler |
| MaxTurnCount | Vidanın ulaşabileceği maksimum tur sayısını belirler |
| ThreadPitch | Bir tam turda vidanın lokal eksende ne kadar ilerleyeceğini belirler |
| RotationAxis | Vidanın hangi lokal eksende döneceğini ve ilerleyeceğini belirler |
| bUseSmoothStop | Limitlere yaklaşırken hız azaltma özelliğini açar veya kapatır |
| SmoothStopPercentage | Son yüzde kaçlık alanda hız azaltılacağını belirler |
| MinimumSmoothSpeedMultiplier | Limitlere yakınken kullanılacak minimum hız çarpanını belirler |

Örnek test değerleri:

```text
RotationSpeed = 360
MaxTurnCount = 5
ThreadPitch = 100
RotationAxis = Z
bUseSmoothStop = true
SmoothStopPercentage = 0.1
MinimumSmoothSpeedMultiplier = 0.15
```

Bu ayarlarda vida saniyede 360 derece döner, maksimum 5 tur ilerler ve her tam turda 100 birim hareket eder.

---

## Debug Bilgileri

Sistem çalışırken ekranda aşağıdaki bilgiler gösterilir:

```text
Current Angle
Current Turn
Current Depth
Direction
```

Örnek debug çıktısı:

```text
Current Angle : 540
Current Turn : 1.5 / 5
Current Depth : 75
Direction : Tightening
```

Limit değerlere ulaşıldığında ekranda şu mesajlar gösterilir:

```text
Fully Tightened
Fully Loosened
```

`Fully Tightened`, vidanın maksimum tur sayısına ulaştığını gösterir.

`Fully Loosened`, vidanın 0 tur değerine ulaştığını gösterir.

---

## Tamamlanan Bonus Özellikler

### Bonus 1 - Event Sistemi

Blueprint tarafından dinlenebilen event yapıları oluşturulmuştur.

Eklenen eventler:

```text
OnScrewFullyTightened
OnScrewFullyLoosened
```

`OnScrewFullyTightened`, vida tamamen sıkıldığında çalışır.

`OnScrewFullyLoosened`, vida tamamen gevşediğinde çalışır.

Bu eventler Blueprint tarafında dinlenebilir ve test amacıyla `Print String`, ses efekti, animasyon veya farklı görsel geri bildirimler için kullanılabilir.

---

### Bonus 2 - Smooth Stop

Vida limit değerlere yaklaşırken dönüş hızı kademeli olarak azaltılmıştır.

Bu özellik sayesinde vida maksimum sıkma veya maksimum gevşetme sınırına sert şekilde çarpmak yerine daha yumuşak şekilde yaklaşır.

Smooth Stop sistemi şu parametrelerle kontrol edilir:

```text
bUseSmoothStop
SmoothStopPercentage
MinimumSmoothSpeedMultiplier
```

Örneğin `SmoothStopPercentage = 0.1` olduğunda, son %10'luk alanda vida hızı kademeli olarak düşürülür.

Hız azaltma işlemi `FMath::Lerp` mantığıyla yapılmıştır.

---

### Bonus 3 - Yardımcı Fonksiyonlar

Vida mekanizmasını dışarıdan kontrol edebilmek için yardımcı fonksiyonlar eklenmiştir.

Eklenen fonksiyonlar:

```text
ResetScrew()
SetScrewTurn(float NewTurn)
GetCurrentTurn()
```

#### ResetScrew()

Vidayı başlangıç durumuna getirir.

```text
CurrentTurn = 0
```

#### SetScrewTurn(float NewTurn)

Vidanın tur değerini dışarıdan ayarlamayı sağlar.

Girilen değer güvenli şekilde sınırlandırılır.

```text
0 <= NewTurn <= MaxTurnCount
```

#### GetCurrentTurn()

Vidanın mevcut tur değerini döndürür.

Ek olarak aşağıdaki yardımcı fonksiyonlar da eklenmiştir:

```text
GetCurrentAngle()
GetCurrentDepth()
```

`GetCurrentAngle()`, mevcut dönüş açısını derece olarak döndürür.

`GetCurrentDepth()`, mevcut ilerleme miktarını döndürür.

---

## Test Süreci

Test için bir Blueprint Actor oluşturulmuştur.

Bu Actor içerisinde:

- Cylinder mesh
- Dönüşü görünür yapmak için küçük marker cube
- ScrewRotatorComponent

bulunmaktadır.

Cylinder mesh ve marker cube `Movable` olarak ayarlanmıştır. Bu sayede runtime sırasında Actor hareket ettirilebilir ve döndürülebilir.

Test sırasında kullanılan örnek ayarlar:

```text
RotationSpeed = 360
MaxTurnCount = 5
ThreadPitch = 100
RotationAxis = Z
```

E tuşuna basıldığında vida sıkma yönünde döner ve ilerler.

Q tuşuna basıldığında vida gevşetme yönünde döner ve geri hareket eder.

Marker cube sayesinde silindirin dönüşü görsel olarak daha net gözlemlenebilir.

---

## Kullanım Adımları

1. Unreal Engine projesini açın.
2. İçinde `UScrewRotatorComponent` bulunan Actor'u sahneye yerleştirin.
3. Actor veya mesh component için `Mobility` değerini `Movable` olarak ayarlayın.
4. `ScrewRotatorComponent` parametrelerini Unreal Editor üzerinden düzenleyin.
5. Play moduna geçin.
6. E tuşu ile vidayı sıkın.
7. Q tuşu ile vidayı gevşetin.
8. Debug değerlerini ve limit mesajlarını ekranda kontrol edin.

---

## Teknik Notlar

- Component, Unreal Actor Component sistemi kullanılarak geliştirilmiştir.
- Ana mekanik C++ tarafında çalışmaktadır.
- Blueprint yalnızca test ve gösterim amacıyla kullanılmıştır.
- Vida hareketi lokal eksene göre hesaplanmaktadır.
- Sistem Actor'un başlangıç transform bilgisini saklar ve vida hareketini bu başlangıç değerine göre uygular.
- Vida hareketi `CurrentTurn` değeri üzerinden kontrol edilir.
- `CurrentTurn`, `0` ile `MaxTurnCount` arasında sınırlandırılır.

---

## Teslim İçeriği

Bu proje tesliminde aşağıdaki içerikler bulunmaktadır:

- Unreal Engine projesi
- C++ kaynak kodları
  - `ScrewRotatorComponent.h`
  - `ScrewRotatorComponent.cpp`
- README dokümanı
- 1-2 dakikalık ekran videosu veya demo video linki
- GitHub repo bağlantısı veya proje arşivi
