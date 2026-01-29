package Testy;

import Loteria.*;
import Loteria.Gracze.*;

import java.util.ArrayList;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
public class Main {
    public static void main(String[] args) {
        MaszynaLosująca rand = new MaszynaLosująca();
        Centrala centrala = new Centrala();
        final int liczbaGraczy = 200;
        final int liczbaLosowań = 20;
        final int liczbaKolektur = 10;

        // Tworzy kolektury.
        Kolektura[] kolektury = new Kolektura[liczbaKolektur];
        for(int i = 0; i < liczbaKolektur; ++i) {
            kolektury[i] = new Kolektura(i, centrala);
            rand.nowaKolektura(kolektury[i]);
        }

        // Tworzy po 200 graczy z każdego rodzaju.
        GraczLosowy[] graczeLosowi = new GraczLosowy[liczbaGraczy];
        for(int i = 0; i < liczbaGraczy; ++i) {
            graczeLosowi[i] = new GraczLosowy(rand.Imię(), rand.Nazwisko(),
                    rand.Pesel(), rand.Złote(), rand.Grosze(), centrala);
            for(int j = 0; j < liczbaKolektur; ++j)
                graczeLosowi[i].nowaKolektura(kolektury[j]);
        }

        GraczStałoliczbowy[] stałoLiczbowi = new GraczStałoliczbowy[liczbaGraczy];
        for(int i = 0; i < liczbaGraczy; ++i){
            int[] zestawIndeksów = rand.zestawIndeksów(liczbaKolektur);
            Kolektura[] ulubioneKolektury = new Kolektura[zestawIndeksów.length];
            for(int j = 0; j < zestawIndeksów.length; ++j)
                ulubioneKolektury[j] = kolektury[zestawIndeksów[j]];
            stałoLiczbowi[i] = new GraczStałoliczbowy(rand.Imię(), rand.Nazwisko(),
                    rand.Pesel(), rand.Złote(), rand.Grosze(), centrala, rand.zestawLiczb(),
                    ulubioneKolektury);
        }

        GraczStałoblankietowy[] stałoBlaniektowi = new GraczStałoblankietowy[liczbaGraczy];
        for(int i = 0; i < liczbaGraczy; ++i){
            int[] zestawIndeksów = rand.zestawIndeksów(liczbaKolektur);
            Kolektura[] ulubioneKolektury = new Kolektura[zestawIndeksów.length];
            for(int j = 0; j < zestawIndeksów.length; ++j)
                ulubioneKolektury[j] = kolektury[zestawIndeksów[j]];

            int liczbaLosowań2 = rand.losowaLiczbaLosowań();
            Blankiet blankiet = new Blankiet();
            blankiet.wybierzLiczbęLosowań(liczbaLosowań2);

            int liczbaZakładów = rand.losowaLiczbaZakładów();
            for(int j = 0; j < liczbaZakładów; ++j){
                int[] mojeLiczby = rand.zestawLiczb();
                for(int k = 0; k < 6; ++k)
                    blankiet.skreśl(mojeLiczby[k], j + 1);
            }
            stałoBlaniektowi[i] = new GraczStałoblankietowy(rand.Imię(),
                    rand.Nazwisko(), rand.Pesel(), rand.Złote(), rand.Grosze(),
                    centrala, blankiet, ulubioneKolektury);
        }

        Minimalista[] minimaliści = new Minimalista[liczbaGraczy];
        for(int j = 0; j < liczbaGraczy; ++j){
            int indeks = rand.indeks(liczbaKolektur);
            Kolektura ulubionaKolektura = kolektury[indeks];
            minimaliści[j] = new Minimalista(rand.Imię(), rand.Nazwisko(),
                    rand.Pesel(), rand.Złote(), rand.Grosze(), centrala,
                    ulubionaKolektura);
        }

        // Przeprowadza losowania.
        for(int nrLosowania = 0; nrLosowania < liczbaLosowań; ++nrLosowania){
            System.out.println("numer losowania: " + nrLosowania);
            for(int j = 0; j < liczbaGraczy; ++j){
                graczeLosowi[j].kupKupon();
                stałoBlaniektowi[j].kupKupon();
                stałoLiczbowi[j].kupKupon();
                minimaliści[j].kupKupon();
            }
            System.out.println("Losowanie");
            centrala.losowanie();
            System.out.println("Gracze odbierają swoje nagrody...\n");
            // Gracze odbierają swoje nagrody.
            sprawdźNagrody(graczeLosowi, centrala, kolektury, liczbaGraczy);
            sprawdźNagrody(stałoBlaniektowi, centrala, kolektury, liczbaGraczy);
            sprawdźNagrody(stałoLiczbowi, centrala, kolektury, liczbaGraczy);
            sprawdźNagrody(minimaliści, centrala, kolektury, liczbaGraczy);
        }

        System.out.println(centrala);
        System.out.println("Zysk państwa z podatków wyniósł: " + centrala.wypiszPodatki());
        System.out.println("Łączne przekazane subwencje: " + centrala.wypiszSubwencje());
    }

    public static void sprawdźNagrody(Gracz[] gracze, Centrala centrala, Kolektura[] kolektury, int liczbaGraczy){
        for(int i = 0; i < liczbaGraczy; ++i){
            ArrayList<Kupon> doSprawdzenia = gracze[i].kuponyDoSprawdzenia(centrala.getNumerLosowania());
            for(Kupon kupon: doSprawdzenia){
                int a = kupon.getPierwszeLosowanie();
                int b = a + kupon.getLiczbaLosowań() - 1;
                boolean czyByłaNagroda = false;
                for(int numerLosowania = a; numerLosowania <= b; ++numerLosowania) {
                    if(gracze[i].sprawdźWygraną(numerLosowania, kupon))
                        czyByłaNagroda = true;
                }
                if(czyByłaNagroda) {
                    int nrKolektury = kupon.getNumerKolektury();
                    PLN nagroda = gracze[i].odbierzNagrodę(kupon, kolektury[nrKolektury]);
                }
            }
            gracze[i].usuńKupony(doSprawdzenia);
        }
    }
}