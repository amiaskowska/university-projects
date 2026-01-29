package Loteria;

import java.util.Arrays;
import java.util.Random;

public class Losowanie {
    private static final int MAX_STOPIEŃ = 4;
    private static final int ILE_LICZB = 49;
    private static final int MAX_LICZB = 6;
    private int numer;
    private int[] wynik;
    private PLN całośćNagród;
    private int[] liczbaWygranych;
    private PLN[] kwotaWygranych;
    private PLN[] pulaWygranych;

    public void setLiczbaWygranych(int[] liczbaWygranych){
        this.liczbaWygranych = liczbaWygranych;
    }

    public void setKwotaWygranych(PLN[] kwotaWygranych){
        this.kwotaWygranych = kwotaWygranych;
    }

    public void setPulaWygranych(PLN[] pulaWygranych){
        this.pulaWygranych = pulaWygranych;
    }

    public void setCałośćNagród(PLN całośćNagród){ this.całośćNagród = całośćNagród.kopia(); }

    public PLN getKwotaWygranej(int stopień){ return kwotaWygranych[stopień - 1].kopia(); }

    public PLN getPulaWygranych(int stopień) { return pulaWygranych[stopień - 1].kopia(); }

    public int getLiczbaWygranych(int stopień){ return liczbaWygranych[stopień - 1]; }

    public Losowanie(int numer){
        this.numer = numer;
        this.wynik = new int[MAX_LICZB];
        liczbaWygranych = new int[MAX_STOPIEŃ];
        kwotaWygranych = new PLN[MAX_STOPIEŃ];
        pulaWygranych = new PLN[MAX_STOPIEŃ];
        boolean czyByła[] = new boolean[ILE_LICZB];

        int ile = 0;
        Random random = new Random();
        while(ile < MAX_LICZB){
            int liczba = random.nextInt(ILE_LICZB);
            if(czyByła[liczba] == false) {
                wynik[ile] = liczba + 1;
                ++ile;
            }
            czyByła[liczba] = true;
        }
        Arrays.sort(wynik);
    }

    // Zwraca zwycięskie liczby (kopia).
    public int[] wyniki(){
        int[] kopia = new int[MAX_LICZB];
        for(int i = 0; i < MAX_LICZB; ++i)
            kopia[i] = wynik[i];
        return kopia;
    }

    // Wydaje podaną kwotę albo całość puli na nagrody, jeśli nie jest
    // wystarczająca.
    public PLN wydajKwotę(PLN kwota){
        PLN doWydania = całośćNagród.min(kwota).kopia();
        całośćNagród.odejmij(doWydania);
        return doWydania;
    }

    public String toString(){
        StringBuilder sb = new StringBuilder();
        sb.append("Losowanie nr " + numer + "\nWyniki: ");
        for(int i = 0; i < MAX_LICZB; ++i){
            if(wynik[i] < 10)
                sb.append(" ");
            sb.append(wynik[i]).append(" ");
        }
        sb.append("\n" + kwotyWygranych() + "\n" + liczbaWygranych())
                .append("\n" + pulaWygranych());
        return sb.toString();
    }

    public int getNumer(){ return numer; }

    private String kwotyWygranych(){
        StringBuilder sb = new StringBuilder();
        sb.append("Kwoty wygranych każdego stopnia, w przeliczeniu na osobę: ")
                .append("\nI stopnia: " + kwotaWygranych[0])
                .append("\nII stopnia: " + kwotaWygranych[1])
                .append("\nIII stopnia: " + kwotaWygranych[2])
                .append("\nIV stopnia: " + kwotaWygranych[3] + "\n");
        return sb.toString();
    }

    private String liczbaWygranych(){
        StringBuilder sb = new StringBuilder();
        sb.append("Liczby wygranych każdego stopnia: ")
                .append("\nI stopnia: " + liczbaWygranych[0])
                .append("\nII stopnia: " + liczbaWygranych[1])
                .append("\nIII stopnia: " + liczbaWygranych[2])
                .append("\nIV stopnia: " + liczbaWygranych[3] + "\n");
        return sb.toString();
    }

    private String pulaWygranych(){
        StringBuilder sb = new StringBuilder();
        sb.append("Pula wygranych każdego stopnia: ")
                .append("\nI stopnia: " + pulaWygranych[0])
                .append("\nII stopnia: " + pulaWygranych[1])
                .append("\nIII stopnia: " + pulaWygranych[2])
                .append("\nIV stopnia: " + pulaWygranych[3] + "\n");
        return sb.toString();
    }
}
