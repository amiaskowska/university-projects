package Loteria;

import Loteria.Gracze.Gracz;

import java.util.ArrayList;
import java.util.Random;

public class Kolektura {
    private final static long CENA_KUPONU = 3;
    private final static int MIN_SKREŚLONYCH = 3;
    private final static int MAX_SKREŚLONYCH = 6;
    private final static int PROCENT_PODATKU = 10;
    private final static int MAX_STOPIEŃ = 4;
    private PLN wysokaNagroda;
    private Centrala centrala;
    private int numer;
    private ArrayList<Kupon> sprzedaneKupony;

    public Kolektura(int numer, Centrala centrala){
        this.numer = numer;
        this.centrala = centrala;
        centrala.nowaKolektura(this);
        wysokaNagroda = new PLN(2280, 0);
        sprzedaneKupony = new ArrayList<>();
    }

    public int getNumer(){ return numer; }

    // Sprawdza, ile padło wygranych danego stopnia.
    public int ileWygranych(int stopień, int[] liczby, int numerLosowania){
        assert stopień >= 0 && stopień < MAX_STOPIEŃ;
        int ile = 0;
        for(Kupon kupon: sprzedaneKupony){

            if(kupon.getCzyZrealizowany() || !kupon.czyWażny(numerLosowania))
                continue;
            for(Pole zakład: kupon.getZakłady()){
                if(zakład.ilePoprawnych(liczby) == (MAX_SKREŚLONYCH - stopień)) {
                    ++ile;
                }
            }
        }
        return ile;
    }

    // Wydaje kupon na podstawie blankietu.
    public Kupon wydajKupon(Gracz gracz, Blankiet blankiet){

        // Sprawdza cenę kuponu oraz czy gracza stać na jego opłacenie.
        int liczbaZakładów = blankiet.ileZakładów();
        int liczbaLosowań = blankiet.ileLosowań();
        PLN cena = cenaKuponu(liczbaZakładów, liczbaLosowań).kopia();
        if(!cena.czyMniejszeRówne(gracz.getŚrodki()))
            return null; // Gracza nie stać na zakup kuponu.
        PLN podatek = cena.podatek();

        // Zbiera dane potrzebne do utworzenia kuponu.
        int numerKuponu = centrala.getNumerKuponu();
        int numerKolektury = this.numer;
        int losowyZnacznik = wygenerujZnacznik();
        String sumaKontrolna = sumaKontrolna(numerKuponu, losowyZnacznik);
        Pole[] zakłady = new Pole[liczbaZakładów];
        for(int i = 0; i < liczbaZakładów; ++i)
            zakłady[i] = blankiet.podajZakład(i);
        int pierwszeLosowanie = centrala.getNumerLosowania();

        // Odbiera zapłatę od gracza i przesyła kwotę do centrali.
        gracz.zapłać(cena);
        centrala.odbierzZapłatę(cena, liczbaLosowań);

        // Tworzy nowy kupon.
        Kupon kupon = new Kupon(numerKuponu, numerKolektury,
                String.format("%09d", losowyZnacznik), sumaKontrolna,
                liczbaZakładów, liczbaLosowań, zakłady, pierwszeLosowanie,
                cena, podatek);

        // Dodaje kopię kuponu na listę sprzedanych kuponów.
        sprzedaneKupony.add(kupon.kopia());

        return kupon;
    }

    // Wydanie kuponu na "chybił-trafił".
    public Kupon wydajKupon(Gracz gracz, int liczbaZakładów, int liczbaLosowań){

        // Sprawdza cenę kuponu oraz czy gracza stać na jego opłacenie.
        PLN cena = cenaKuponu(liczbaZakładów, liczbaLosowań).kopia();
        if(!cena.czyMniejszeRówne(gracz.getŚrodki()))
            return null; // Gracza nie stać na zakup kuponu.
        PLN podatek = cena.podatek();

        // Zbiera dane potrzebne do utworzenia kuponu.
        int numerKuponu = centrala.getNumerKuponu();
        int numerKolektury = this.numer;
        int losowyZnacznik = wygenerujZnacznik();
        String sumaKontrolna = sumaKontrolna(numerKuponu, losowyZnacznik);
        Pole[] zakłady = new Pole[liczbaZakładów];
        for(int i = 0; i < liczbaZakładów; ++i){
            zakłady[i] = new Pole();
            zakłady[i].przypiszLosowo();
        }
        int pierwszeLosowanie = centrala.getNumerLosowania();

        // Odbiera zapłatę od gracza i przesyła kwotę do centrali.
        gracz.zapłać(cena);
        centrala.odbierzZapłatę(cena, liczbaLosowań);

        // Tworzy nowy kupon.
        Kupon kupon = new Kupon(numerKuponu, numerKolektury,
            String.format("%09d", losowyZnacznik), sumaKontrolna,
            liczbaZakładów, liczbaLosowań, zakłady, pierwszeLosowanie,
            cena, podatek);

        // Dodaje kopię kuponu na listę sprzedancyh kuponów.
        sprzedaneKupony.add(kupon.kopia());
        return kupon;
    }

    // Poddaje kupon weryfikacji i wypłaca odpowiednią nagrodę.
    public PLN odbiórWygranej(Gracz gracz, Kupon kupon){
        boolean czyKupionyTutaj = false;
        boolean czyZrealizowany = true;
        boolean czyDanePoprawne = true;

        // Sprawdza, czy kupon był wydany w tej kolekturze, czy nie został
        // jeszcze zrealizowany oraz czy dane wydruku są poprawne.
        Kupon kuponWBazie = null;
        for(Kupon sprzedanyKupon: sprzedaneKupony){
            if(sprzedanyKupon.identyfikator().equals(kupon.identyfikator())){
                kuponWBazie = sprzedanyKupon;
                czyKupionyTutaj = true;
                if(!sprzedanyKupon.getCzyZrealizowany())
                    czyZrealizowany = false;
                if(!sprzedanyKupon.wydruk().equals(kupon.wydruk()))
                    czyDanePoprawne = false;
            }
        }

        // Nagroda nie przysługuje temu graczowi.
        if(czyKupionyTutaj == false || czyZrealizowany == true
            || czyDanePoprawne == false)
            return new PLN(0, 0);

        // Wyznacza wartość wszystkich nagród za wszystkie odbyte losowania,
        // które są objęte kuponem.
        int a = kupon.getPierwszeLosowanie();
        int b = kupon.getPierwszeLosowanie() + kupon.getLiczbaLosowań() - 1;
        Pole[] zakłady = kupon.getZakłady();
        boolean czyOpodatkowana = false;
        PLN łącznaNagroda = new PLN(0, 0);
        PLN wartośćWysokiejNagrody = new PLN(0,0);

        for(int numerLosowania = a; numerLosowania <= b; ++numerLosowania){

            int[] zwycięskieLiczby = centrala.zwycięskieLiczby(numerLosowania);
            for(Pole zakład: zakłady){
                int ilePoprawnych = zakład.ilePoprawnych(zwycięskieLiczby);
                if(ilePoprawnych < MIN_SKREŚLONYCH) continue;

                int stopieńWygranej = MAX_SKREŚLONYCH - ilePoprawnych + 1;
                PLN nagroda = centrala.wydajNagrodę(stopieńWygranej, numerLosowania);

                // Sprawddza, czy nagroda kwalifikuje się jako wysoka.
                if(nagroda.czyMniejsze(wysokaNagroda) == false) {
                    czyOpodatkowana = true;
                    wartośćWysokiejNagrody = new PLN(nagroda.getZłote(), nagroda.getGrosze());
                }
                łącznaNagroda.dodaj(nagroda);
            }
        }

        // Odprowadza podatek od nagrody, o ile obowiązuje (odprowadzam
        // podatek od tylko jednej wysokiej nagrody, jeśli nagród było kilka).
        if(czyOpodatkowana) {
            PLN wartośćPodatku = wartośćWysokiejNagrody.kopia();
            wartośćPodatku.dajProcent(PROCENT_PODATKU);
            łącznaNagroda.odejmij(wartośćPodatku);
            centrala.odbierzPodatek(wartośćPodatku);
        }

        // Oznacza kupon w bazie jako zrealizowany - usuwa z listy kuponów.
        sprzedaneKupony.remove(kuponWBazie);
        return łącznaNagroda;
    }

    // Usuwa z listy sprzedanych kuponów te, które są już zrealizowane
    // albo są przedawnione i nie było na nie żadnej nagrody.
    public void wyczyśćListęKuponów(int nrLosowania){
        ArrayList<Kupon> kuponyDoUsunięcia = new ArrayList<>();
        for(Kupon kupon: sprzedaneKupony){
            if(kupon.getCzyZrealizowany())
                kuponyDoUsunięcia.add(kupon);
            else if(!kupon.czyWażny(nrLosowania)){
                boolean czyWygrany = false;
                int a = kupon.getPierwszeLosowanie();
                int b = a + kupon.getLiczbaLosowań() - 1;
                for(int numer = a; numer <= b; ++numer){
                    if(kupon.czyWygrany(centrala.zwycięskieLiczby(numer))){
                        czyWygrany = true;
                        break;
                    }
                }
                if(czyWygrany == false)
                    kuponyDoUsunięcia.add(kupon);
            }
        }
        for(Kupon kupon: kuponyDoUsunięcia)
            sprzedaneKupony.remove(kupon);
    }

    private PLN cenaKuponu(int liczbaZakładów, int liczbaLosowań){
        PLN cena = new PLN(CENA_KUPONU, 0);
        cena.przemnóż(liczbaZakładów * liczbaLosowań);
        return cena;
    }

    // Generuje znacznik losowy.
    private int wygenerujZnacznik(){
        Random random = new Random();
        int liczba = random.nextInt(1000000000);
        //String kod = String.format("%09d", liczba);
        return liczba;
    }

    // Generuje sumę kontrolną.
    private String sumaKontrolna(int numerKuponu, int znacznik){
        int suma = sumaCyfr(numer) + sumaCyfr(numerKuponu) + sumaCyfr(znacznik);
        suma %= 100;
        String kod = String.format("%02d", suma);
        return kod;
    }

    private int sumaCyfr(int liczba){
        int suma = 0;
        while(liczba > 0){
            suma += liczba % 10;
            liczba /= 10;
        }
        return suma;
    }
}
