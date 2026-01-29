package Loteria;

public class Blankiet {
    private final static int MAX_LICZBA_ZAKŁADÓW = 8;
    private final static int MAX_LICZBA_LOSOWAŃ = 10;
    private final static int ILE_LICZB = 6;
    private Pole[] zakłady;
    private boolean[] liczbaLosowań;

    public Blankiet(){
        zakłady = new Pole[MAX_LICZBA_ZAKŁADÓW];
        for(int i = 0; i < MAX_LICZBA_ZAKŁADÓW; ++i)
            zakłady[i] = new Pole();
        liczbaLosowań = new boolean[MAX_LICZBA_LOSOWAŃ];
    }

    public Blankiet(Pole[] zakłady, boolean[] liczbaLosowań){
        this.zakłady = zakłady;
        this.liczbaLosowań = liczbaLosowań;
    }

    public Blankiet kopia(){
        Pole[] zakładyKopia = new Pole[MAX_LICZBA_ZAKŁADÓW];
        for(int i = 0; i < MAX_LICZBA_ZAKŁADÓW; ++i)
            zakładyKopia[i] = zakłady[i].kopia();
        boolean[] liczbaLosowańKopia = new boolean[MAX_LICZBA_LOSOWAŃ];
        for(int i = 0; i < MAX_LICZBA_LOSOWAŃ; ++i)
            liczbaLosowańKopia[i] = liczbaLosowań[i];
        Blankiet kopia = new Blankiet(zakładyKopia, liczbaLosowańKopia);
        return kopia;
    }

    // Skreśl podaną liczbę na podanym zakładzie.
    public void skreśl(int liczba, int numerZakładu){
        assert numerZakładu >= 1 &&  numerZakładu <= MAX_LICZBA_ZAKŁADÓW;
        zakłady[numerZakładu - 1].skreślLiczbę(liczba);
    }

    public void anulujZakład(int numerZakładu){
        assert numerZakładu >= 1 && numerZakładu <= MAX_LICZBA_ZAKŁADÓW;
        zakłady[numerZakładu - 1].anuluj();
    }

    public void wybierzLiczbęLosowań(int liczba){
        assert liczba >= 1 && liczba <= MAX_LICZBA_LOSOWAŃ;
        liczbaLosowań[liczba - 1] = true;
    }

    public int ileSkreślonych(int numerZakładu){
        assert numerZakładu >= 1 && numerZakładu <= MAX_LICZBA_ZAKŁADÓW;
        return zakłady[numerZakładu - 1].ileSkreślonych();
    }

    // Sprawdza, ile ważnych i nieanulowanych zakładów zawiera blankiet.
    public int ileZakładów(){
        int ile = 0;
        for(int i = 0; i < MAX_LICZBA_ZAKŁADÓW; ++i){
            if(!zakłady[i].czyAnulowane() && ileSkreślonych(i + 1) == ILE_LICZB)
                ++ile;
       }
        return ile;
    }

    // Sprawdza, na ile losowań przysługuje kupon według danych z blankietu,
    // czyli zwraca największą zaznaczoną liczbę albo jeden.
    public int ileLosowań(){
        for(int i = MAX_LICZBA_LOSOWAŃ - 1; i >= 0; --i){
            if(liczbaLosowań[i] == true)
                return i + 1;
        }
        return 1;
    }

    // Zwraca pole z i-tym ważnym i nieanulowanym zakładem (kopia).
    public Pole podajZakład(int i){
        assert i >= 1 && i <= MAX_LICZBA_ZAKŁADÓW;
        int licznik = 0;
        for(int j = 0; j < MAX_LICZBA_ZAKŁADÓW; ++j){
            if(!zakłady[j].czyAnulowane() && ileSkreślonych(j + 1) == ILE_LICZB){
                if(licznik == i)
                    return zakłady[j].kopia();
                ++licznik;
            }
        }
        return null;
    }
}
