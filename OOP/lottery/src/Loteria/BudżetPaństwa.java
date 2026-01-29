package Loteria;

public class BudżetPaństwa {
    private PLN pobranePodatki;
    private PLN przekazaneSubwencje;

    public BudżetPaństwa(){
        pobranePodatki = new PLN(0,0);
        przekazaneSubwencje = new PLN(0,0);
    }

    public String wypiszPodatki(){
        return pobranePodatki.toString();
    }

    public String wypiszSubwencje(){
        return przekazaneSubwencje.toString();
    }

    public void odbierzPodatek(PLN podatek){
        pobranePodatki.dodaj(podatek);
    }

    public void przekażSubwencję(PLN c){
        przekazaneSubwencje.dodaj(c);
    }
}
