package Loteria;

public class PLN {
    private long złote, grosze;

    public PLN(long złote, long grosze){
        this.złote = złote;
        this.grosze = grosze;
    }

    public String toString(){
        StringBuilder sb = new StringBuilder();
        sb.append(złote).append(" zł ");
        if(grosze < 10)
            sb.append("0");
        sb.append(grosze).append(" gr");
        return sb.toString();
    }

    public long getZłote(){
        return złote;
    }

    public long getGrosze(){
        return grosze;
    }

    public PLN kopia(){
        PLN kopia = new PLN(złote, grosze);
        return kopia;
    }

    // Zwraca podatek z obecnej kwoty.
    public PLN podatek(){
        long całość = grosze + złote * 100;
        całość /= 5; // 20% z całości.
        PLN podatek = new PLN(całość/100, całość%100);
        return podatek;
    }

    // Oblicza procent% z obecnej kwoty.
    public PLN dajProcent(int procent){
        long całość = grosze + złote * 100;
        całość = (całość * procent) / 100;
        PLN wynik = new PLN(całość / 100, całość % 100);
        return wynik;
    }

    public void przemnóż(int m){
        złote *= m;
        grosze *= m;
        złote += grosze / 100;
        grosze %= 100;
    }

    public void odejmij(PLN c){
        assert c.czyMniejszeRówne(this);
        long całość = grosze + złote * 100;
        long całość2 = c.getGrosze() + c.getZłote() * 100;
        całość -= całość2;
        złote = całość / 100;
        grosze = całość % 100;
    }

    public void dodaj(PLN c){
        long całość = grosze + złote * 100;
        long całość2 = c.getGrosze() + c.getZłote() * 100;
        całość += całość2;
        złote = całość / 100;
        grosze = całość % 100;
    }

    public void podziel(long m){
        assert m != 0;
        long całość = grosze + złote * 100;
        całość /= m;
        złote = całość / 100;
        grosze = całość % 100;
    }

    public PLN min(PLN c){
        if(this.czyMniejszeRówne(c))
            return this;
        else return c;
    }


    // Sprawdza, czy obecna kwota jest mniejsza lub równa od podanego c.
    public boolean czyMniejszeRówne(PLN c){
        long całość1 = this.grosze + this.złote * 100;
        long całość2 = c.getGrosze() + c.getZłote() * 100;
        if(całość1 <= całość2)
            return true;
        else
            return false;
    }

    // Sprawdza, czy obecna kwota jest mniejsza od podanego c.
    public boolean czyMniejsze(PLN c){
        long całość1 = this.grosze + this.złote * 100;
        long całość2 = c.getGrosze() + c.getZłote() * 100;
        if(całość1 < całość2)
            return true;
        else
            return false;
    }
}
