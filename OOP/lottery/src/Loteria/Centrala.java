package Loteria;

import java.util.ArrayList;

public class Centrala {
    private final static int MAX_STOPIEŃ = 4;
    private final static int MAX_LICZBA_LOSOWAŃ = 10;
    private final static long MINIMALNA_NAGRODA_I = 2000000;
    private final static long MINIMALNA_NAGRODA_III = 36;
    private final static long NAGRODA_IV = 24;
    private BudżetPaństwa państwo;
    private PLN środkiFinansowe; // Czyli zysk/awaryjne środki na nagrody, bez podatku.
    private PLN[] wpłatyZZakładów; // Suma opłat za kupony na najbliższe 10 losowań, bez podatku.
    private PLN kumulacja; // Wartość obecnej kumulacji.
    private int numerKuponu; // Najmniejszy dostępny numer kuponu.
    private int numerLosowania; // Numer najbliższego losowania.
    private ArrayList<Kolektura> kolektury;
    private ArrayList<Losowanie> losowania;

    public Centrala(){
        this.państwo = new BudżetPaństwa();
        środkiFinansowe = new PLN(0,0);

        wpłatyZZakładów = new PLN[MAX_LICZBA_LOSOWAŃ];
        for(int i = 0; i < MAX_LICZBA_LOSOWAŃ; ++i)
            wpłatyZZakładów[i] = new PLN(0,0);
        
        numerKuponu = 0;
        numerLosowania = 0;
        kumulacja = new PLN(0,0);
        kolektury = new ArrayList<>();
        losowania = new ArrayList<>();
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        for(int i = 0; i < losowania.size(); ++i)
            sb.append("______\n" + wynikLosowania(i) + "\n");
        return sb.toString();
    }

    // Dodaje kolekturę na listę.
    public void nowaKolektura(Kolektura kolektura){
        kolektury.add(kolektura);
    }

    // Wypisuje wszystkie wymagane dane związane z losowaniem numer nr.
    public String wynikLosowania(int nr){
        return losowania.get(nr).toString();
    }

    // Zwraca następny dostępny numer kuponu i oznacza go jako użyty.
    public int getNumerKuponu(){
        ++numerKuponu;
        return numerKuponu - 1;
    }

    // Zwraca numer najbliższego losowania.
    public int getNumerLosowania(){
        return numerLosowania;
    }
    
    // Centrala udostępnia informacje dotyczące zwycięskich liczb w podanym
    // losowaniu (zwracana jest kopia liczb).
    public int[] zwycięskieLiczby(int numerLosowania){
        return losowania.get(numerLosowania).wyniki();
    }

    public String wypiszPodatki(){
        return państwo.wypiszPodatki();
    }

    public String wypiszSubwencje(){
        return państwo.wypiszSubwencje();
    }

    // Centrala odbiera pieniądze od kolektury za zakup kuponu i przesyła
    // podatek do państwa.
    public void odbierzZapłatę(PLN kwota, int ileLosowań){
        PLN podatek = kwota.podatek();
        kwota.odejmij(podatek);
        państwo.odbierzPodatek(podatek);

        // Kwota jest rozdzielana po równo do puli nagród za wszystkie losowaia,
        // które były objęte kuponem.
        kwota.podziel(ileLosowań);
        for(int i = 0; i < ileLosowań; ++i){
            wpłatyZZakładów[i].dodaj(kwota);
        }
    }

    // Centrala odbiera podatek od nagrody i przesyła go do państwa.
    public void odbierzPodatek(PLN podatek){
        państwo.odbierzPodatek(podatek);
    }

    // Centrala wydaje odpowiednią nagrodę k-tego stopnia.
    public PLN wydajNagrodę(int k, int numerLosowania){
        if(numerLosowania >= this.numerLosowania)
            return new PLN(0,0);
        Losowanie losowanie = losowania.get(numerLosowania);
        znajdźPieniądze(losowanie.getKwotaWygranej(k), losowanie);
        return losowanie.getKwotaWygranej(k);
    }

    // Centrala pobiera odpowiednią sumę na pokrycie nagrody, priorytetowo
    // z puli na nagrody, następnie ze swojego zysku, końcowo z budżetu państwa.
    private void znajdźPieniądze(PLN kwota, Losowanie losowanie){
        PLN brakuje = kwota.kopia();
        brakuje.odejmij(losowanie.wydajKwotę(kwota));
        brakuje.odejmij(środkiFinansowe.min(brakuje));
        państwo.przekażSubwencję(brakuje);
    }

    // Centrala przepowadza losowanie, oblicza wartości nagród i swój zysk
    // oraz udostępnia zwycięskie liczby.
    public int[] losowanie(){
        Losowanie losowanie = new Losowanie(numerLosowania);
        losowania.add(losowanie);
        ++numerLosowania;

        aktualizujLiczbęWygranych(losowanie);
        PLN całośćNagród = obliczPulęNagród();
        losowanie.setCałośćNagród(całośćNagród.kopia());
        aktualizujPulęNagród(losowanie, całośćNagród);
        aktualizujKwotęWygranych(losowanie);
        aktualizujKumulację(losowanie);
        pobierzZysk(losowanie);
        aktualizujKolektury(numerLosowania);

        return losowanie.wyniki();
    }

    // Pobiera 51% wpłat z zakładów jako pula nagród, reszta jako zysk.
    private PLN obliczPulęNagród(){
        PLN całośćNagród = wpłatyZZakładów[0].dajProcent(51);
        wpłatyZZakładów[0].odejmij(całośćNagród);
        środkiFinansowe.dodaj(wpłatyZZakładów[0]);
        przesuńTablicę();
        return całośćNagród;
    }

    // Przesuwa wpłaty z zakładów o jeden w lewo, usuwając informacje
    // o wpłatach na odbyte już losowanie.
    private void przesuńTablicę(){
        for(int i = 0; i < 9; ++i){
            wpłatyZZakładów[i] = wpłatyZZakładów[i + 1];
        }
        wpłatyZZakładów[9] = new PLN(0, 0);
    }

    // Czyści kolektury z kuponów, które nie wygrywają niczego w losowaniu.
    private void aktualizujKolektury(int nrLosowania){
        for(Kolektura kolektura: kolektury)
            kolektura.wyczyśćListęKuponów(nrLosowania);
    }

    // Jeśli nie padła nagroda stopnia I, to zwiększana jest kumulacja.
    private void aktualizujKumulację(Losowanie losowanie){
        if(losowanie.getLiczbaWygranych(1) == 0)
            kumulacja.dodaj(losowanie.getPulaWygranych(1));
    }

    // Jeśli nie padła nagroda stopnia wyższego niż I, to pula na tę
    // nagrodę przekazywana jest do zysku centrali.
    private void pobierzZysk(Losowanie losowanie){
        for(int i = 2; i <= MAX_STOPIEŃ; ++i){
            if(losowanie.getLiczbaWygranych(i) == 0)
                środkiFinansowe.dodaj(losowanie.getPulaWygranych(i));
        }
    }

    // Zbiera ze wszystkich kolektur informacje, ile zakładów wygrało
    // nagrodę i-tego stopnia.
    private void aktualizujLiczbęWygranych(Losowanie losowanie){
        int[] liczbaWygranych = new int[MAX_STOPIEŃ];
        for(Kolektura kolektura: kolektury){
            for(int i = 0; i < MAX_STOPIEŃ; ++i){
                int ileWygranych = kolektura.ileWygranych(i, losowanie.wyniki(), losowanie.getNumer());
                liczbaWygranych[i] += ileWygranych;
            }
        }
        losowanie.setLiczbaWygranych(liczbaWygranych);
    }

    // Oblicza pulę nagród każdego stopnia, z uwzględnieniem, by pobrać
    // pieniądze z odpowiednich puli (środki z kuponów/zysk centrali/państwo).
    private void aktualizujPulęNagród(Losowanie losowanie, PLN całośćNagród){
        PLN[] pulaNagrody = new PLN[MAX_STOPIEŃ];
        PLN pozostałeŚrodki = całośćNagród.kopia();
        pulaNagrody[0] = nagrodaStI(całośćNagród, pozostałeŚrodki);
        pulaNagrody[1] = nagrodaStII(całośćNagród, pozostałeŚrodki);
        pulaNagrody[3] = nagrodaStIV(pozostałeŚrodki, losowanie.getLiczbaWygranych(4));
        pulaNagrody[2] = nagrodaStIII(pozostałeŚrodki, losowanie.getLiczbaWygranych(3));
        losowanie.setPulaWygranych(pulaNagrody);
    }

    // Przelicza wysokość nagrody dla jednej osoby.
    private void aktualizujKwotęWygranych(Losowanie losowanie){
        PLN[] kwotaNagrody = new PLN[MAX_STOPIEŃ];
        for(int i = 0; i < MAX_STOPIEŃ; ++i){
            kwotaNagrody[i] = losowanie.getPulaWygranych(i + 1);
            if(losowanie.getLiczbaWygranych(i + 1) != 0)
                kwotaNagrody[i].podziel(losowanie.getLiczbaWygranych(i + 1));
        }
        losowanie.setKwotaWygranych(kwotaNagrody);
    }

    // Oblicza pulę nagrody stopnia I - 44% z puli na nagrody, z gwarancją
    // minimum 2 mln, zwiększone o kumulację.
    private PLN nagrodaStI(PLN pulaNagrody, PLN pozostałeŚrodki){
        PLN nagroda = pulaNagrody.dajProcent(44);
        pozostałeŚrodki.odejmij(nagroda);
        if(nagroda.czyMniejsze(new PLN(MINIMALNA_NAGRODA_I, 0)))
            nagroda = new PLN(MINIMALNA_NAGRODA_I, 0);

        nagroda.dodaj(kumulacja);
        kumulacja = new PLN(0, 0);
        return nagroda;
    }

    // Oblicza pulę nagrody stopnia II - 8% z puli na nagrody.
    private PLN nagrodaStII(PLN pulaNagrody, PLN pozostałeŚrodki){
        PLN nagroda = pulaNagrody.dajProcent(8);
        pozostałeŚrodki.odejmij(nagroda);
        return nagroda;
    }

    // Oblicza pulę nagrody stopnia III - pozostałe środki z puli na nagrody,
    // z gwarancją co najmniej 36zł na osobę.
    private PLN nagrodaStIII(PLN pozostałeŚrodki, int liczbaWygranych){
        PLN minimalnaPula = new PLN(MINIMALNA_NAGRODA_III, 0);
        minimalnaPula.przemnóż(liczbaWygranych);
        if(pozostałeŚrodki.czyMniejsze(minimalnaPula))
            return minimalnaPula;
        else
            return pozostałeŚrodki;
    }

    // Oblicza pulę nagrody stopnia IV - 24zł za każdy wygrany zakład.
    private PLN nagrodaStIV(PLN pozostałeŚrodki, int liczbaWygranych){
        PLN nagroda = new PLN(NAGRODA_IV, 0);
        nagroda.przemnóż(liczbaWygranych);
        if(nagroda.czyMniejszeRówne(pozostałeŚrodki))
            pozostałeŚrodki.odejmij(nagroda);
        else
            pozostałeŚrodki = new PLN(0,0);
        return nagroda;
    }

}
