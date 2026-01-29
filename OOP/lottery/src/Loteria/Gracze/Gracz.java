package Loteria.Gracze;

import Loteria.Centrala;
import Loteria.Kolektura;
import Loteria.Kupon;
import Loteria.PLN;

import java.util.ArrayList;
import java.util.Arrays;

public abstract class Gracz {
    private Centrala centrala;
    private String imię;
    private String nazwisko;
    private String PESEL;
    private PLN środki;
    private boolean brakKuponów = true;
    private ArrayList<Kupon> mojeKupony;

    public Gracz(String imię, String nazwisko, String PESEL, long złote, long grosze, Centrala centrala){
        this.imię = imię;
        this.nazwisko = nazwisko;
        this.PESEL = PESEL;
        this.środki = new PLN(złote, grosze);
        this.centrala = centrala;
        mojeKupony = new ArrayList<>();
    }

    public PLN getŚrodki(){
        return środki.kopia();
    }

    public int getLiczbaKuponów(){
        return mojeKupony.size();
    }

    public Centrala getCentrala(){
        return centrala;
    }

    public Kupon getKupon(int indeks){
        if(mojeKupony.size() == 0)
            return null;
        return mojeKupony.get(indeks);
    }

    public String getImię(){ return imię; }
    public String getNazwisko(){ return nazwisko; }
    public String toString(){
        StringBuilder sb = new StringBuilder();
        sb.append(nazwisko+ " " + imię + "\n PESEL: " + PESEL)
            .append("\n Posiadane środki: " + środki);
        return sb.toString();
    }

    // Gracz opłaca kupon.
    public void zapłać(PLN cena){
        środki.odejmij(cena);
    }

    // Dla podanego kuponu i numeru losowania sprawdza, czy graczowi przysługuje
    // jakaś wygrana w ramach tego kuponu.
    public boolean sprawdźWygraną(int nrLosowania, Kupon kupon){
        boolean czyZwycięski = false;
        int[] wyniki = centrala.zwycięskieLiczby(nrLosowania);
        for(int i = 3; i <= 6; ++i){
            // Sprawdza, ile jest wygranych zakładów na tym kuponie, gdzie
            // poprawnych było dokładnie i liczb.
            if(kupon.ileWygranych(wyniki, i) > 0)
                czyZwycięski = true;
        }
        return czyZwycięski;
    }

    // Zwraca nagrodę (bez podatku) albo zero, jeśli na ten kupon nie przysługuje
    // żadna nagroda.
    public PLN odbierzNagrodę(Kupon kupon, Kolektura kolektura){
        PLN mojaNagroda = kolektura.odbiórWygranej(this, kupon).kopia();
        środki.dodaj(mojaNagroda);
        kupon.zrealizujKupon();
        return mojaNagroda;
    }

    // Dodaje zakupiony kupon na listę kuponów.
    public void dodajKupon(Kupon kupon){
        mojeKupony.add(kupon);
    }

    // Usuwa sprawdzone kupony z listy kuponów.
    public void usuńKupony(ArrayList<Kupon> kupony){
        for(Kupon kupon: kupony)
            mojeKupony.remove(kupon);
    }

    // Zwraca wszystkie kupony, które nie zostały jeszcze zrealizowane,
    // a których losowania zostały już wykonane oraz usuwa z listy te,
    // które się przedawniły, a nie ma na nich nagrody.
    public ArrayList<Kupon> kuponyDoSprawdzenia(int nrLosowania){
        ArrayList<Kupon> kupony = new ArrayList<>();
        ArrayList<Kupon> kuponyDoUsunięcia = new ArrayList<>();

        for(Kupon kupon: mojeKupony){
            if(kupon.getCzyZrealizowany())
                kuponyDoUsunięcia.add(kupon);
            else if(!kupon.czyWażny(nrLosowania)){
                boolean czyWygrany = czyWygrany(kupon);
                if(czyWygrany)
                    kupony.add(kupon);
                else
                    kuponyDoUsunięcia.add(kupon);
            }
        }

        usuńKupony(kuponyDoUsunięcia);
        return kupony;
    }

    // Sprawdza, czy na całym kuponie była jakaś nagroda.
    private boolean czyWygrany(Kupon kupon) {
        int a = kupon.getPierwszeLosowanie();
        int b = a + kupon.getLiczbaLosowań() - 1;
        boolean czyWygrany = false;
        for(int i = a; i <= b; ++i){
            if(sprawdźWygraną(i, kupon)) {
                czyWygrany = true;
                break;
            }
        }
        return czyWygrany;
    }

    public abstract void kupKupon();

}
