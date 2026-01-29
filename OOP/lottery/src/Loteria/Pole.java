package Loteria;

import java.util.Random;

public class Pole {
    private boolean[] skreślone;
    private boolean anuluj;

    public Pole(){
        skreślone = new boolean[49];
        anuluj = false;
    }

    // Zwraca głęboką kopię tego pola.
    public Pole kopia(){
        Pole kopia = new Pole();
        kopia.skreślone = this.skreślone.clone();
        kopia.anuluj = this.anuluj;
        return kopia;
    }

    public String toString(){
        StringBuilder sb = new StringBuilder();
        for(int i = 0 ; i < 49; ++i){
            if(skreślone[i]){
                if((i + 1) < 10)
                    sb.append(" ");
                sb.append(i + 1).append(" ");

            }
        }
        return sb.toString();
    }

    // Zwraca, ile liczb skreślonych zgadza się z liczbami.
    public int ilePoprawnych(int[] liczby){
        assert liczby.length == 6;
        int i = 0;
        int licznik = 0;
        for(int j = 1; j <= 49; ++j){
            if(liczby[i] < j) ++i;
            if(i == 6)
                break;
            if(skreślone[j - 1] && (liczby[i]) == j)
                ++licznik;
        }
        return licznik;
    }

    // Skreśla 6 losowych liczb.
    public void przypiszLosowo(){
        int ileLiczb = 0;
        Random random = new Random();
        while(ileLiczb < 6){
            int liczba = random.nextInt(49);
            if(!skreślone[liczba])
                ++ileLiczb;
            skreślone[liczba] = true;
        }
    }

    // Skreśla kratkę z podaną liczbą.
    public void skreślLiczbę(int liczba){
        assert liczba >= 1 && liczba <= 49;
        skreślone[liczba - 1] = true;
    }

    // Zwraca, ile kratek liczbowych zostało skreślonych.
    public int ileSkreślonych(){
        int ile = 0;
        for(int i = 0; i < 49; ++i){
            if(skreślone[i])
                ++ile;
        }
        return ile;
    }

    // Skreśla kratkę "anuluj".
    public void anuluj(){
        anuluj = true;
    }

    // Zwraca, czy kratka "anuluj" została skreślona.
    public boolean czyAnulowane(){
        return anuluj;
    }
}
