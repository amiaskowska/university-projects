package Loteria;

public class Kupon {
    private final static int MIN_SKREŚLONYCH = 3;
    private final static int MAX_SKREŚLONYCH = 6;
    private int numerKuponu;
    private int numerKolektury;
    private String losowyZnacznik;
    private String sumaKontrolna;
    private int liczbaZakładów;
    private int liczbaLosowań;
    private Pole[] zakłady;
    private int pierwszeLosowanie; // Nr 1. losowania, któe obejmuje kupon.
    private PLN cena;
    private PLN podatek;
    private boolean czyZrealizowany = false;

    public Kupon(int numerKuponu, int numerKolektury, String losowyZnacznik,
                 String sumaKontrolna, int liczbaZakładów, int liczbaLosowań,
                 Pole[] zakłady, int pierwszeLosowanie, PLN cena, PLN podatek){
        this.numerKuponu = numerKuponu;
        this.numerKolektury = numerKolektury;
        this.losowyZnacznik = losowyZnacznik;
        this.sumaKontrolna = sumaKontrolna;
        this.liczbaZakładów = liczbaZakładów;
        this.liczbaLosowań = liczbaLosowań;
        this.zakłady = zakłady;
        this.pierwszeLosowanie = pierwszeLosowanie;
        this.cena = cena;
        this.podatek = podatek;
    }

    public boolean getCzyZrealizowany(){
        return czyZrealizowany;
    }

    public Pole[] getZakłady(){
        return zakłady;
    }

    public int getPierwszeLosowanie(){ return pierwszeLosowanie; }

    public int getLiczbaLosowań(){ return liczbaLosowań; }

    public int getNumerKolektury(){ return numerKolektury; };

    public void zrealizujKupon(){ czyZrealizowany = true; }


    // Sprawdza, ile zakładów z kuponu miało dokładnie k poprawnie skreślonych.
    public int ileWygranych(int[] liczby, int k){
        int licznik = 0;
        for(int i = 0; i < liczbaZakładów; ++i){
            if(zakłady[i].ilePoprawnych(liczby) == k)
                ++licznik;
        }
        return licznik;
    }

    // Zwraca, czy na kuponie znajduje się jakaś wygrana dla
    // podanego zestawu liczb.
    public boolean czyWygrany(int[] liczby) {
        boolean czyWygrany = false;
        for(int i = MIN_SKREŚLONYCH; i <= MAX_SKREŚLONYCH; ++i){
            if(ileWygranych(liczby, i) > 0)
                czyWygrany = true;
        }
        return czyWygrany;
    }

    // Zwraca, czy kupon obejmuje jeszcze podane losowanie.
    public boolean czyWażny(int nrLosowania){
        int a = pierwszeLosowanie;
        int b = a + liczbaLosowań - 1;
        if(a <= nrLosowania && nrLosowania <= b)
            return true;
        else return false;
    }

    // Zwraca głęboką kopię kuponu.
    public Kupon kopia(){
        Pole[] zakładyKopia = new Pole[zakłady.length];
        for(int i = 0; i < zakłady.length; ++i){
            zakładyKopia[i] = zakłady[i].kopia();
        }

        PLN cenaKopia = cena.kopia();
        PLN podatekKopia = podatek.kopia();

        Kupon kopia = new Kupon(numerKuponu, numerKolektury, losowyZnacznik,
                sumaKontrolna, liczbaZakładów, liczbaLosowań, zakładyKopia,
                pierwszeLosowanie, cenaKopia, podatekKopia);
        return kopia;
    }

    // Tworzy identyfikator kuponu.
    public String identyfikator(){
        StringBuilder sb = new StringBuilder();
        sb.append(numerKuponu + "-" + numerKolektury + "-")
                .append(losowyZnacznik + "-" + sumaKontrolna);
        return sb.toString();
    }

    public String wydruk(){
        StringBuilder sb = new StringBuilder();
        sb.append("KUPON NR ").append(identyfikator()).append("\n");
        for(int i = 0; i < liczbaZakładów; ++i){
            sb.append(i + 1).append(": ").append(zakłady[i]).append("\n");
        }
        sb.append("LICZBA LOSOWAŃ: ").append(liczbaLosowań).append("\n");
        sb.append("NUMERY LOSOWAŃ: ");
        for(int i = 0; i < liczbaLosowań; ++i){
            sb.append(pierwszeLosowanie + i).append(" ");
        }
        sb.append("\n").append("CENA: ").append(cena);
        return sb.toString();
    }


}
