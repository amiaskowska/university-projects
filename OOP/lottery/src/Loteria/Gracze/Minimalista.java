package Loteria.Gracze;

import Loteria.Centrala;
import Loteria.Kolektura;
import Loteria.Kupon;

public class Minimalista extends Gracz {
    private Kolektura ulubionaKolektura;

    public Minimalista(String imię, String nazwisko, String PESEL, long złote, long grosze,
                       Centrala centrala, Kolektura ulubionaKolektura) {
        super(imię, nazwisko, PESEL, złote, grosze, centrala);
        this.ulubionaKolektura = ulubionaKolektura;
    }

    @Override
    public void kupKupon(){
        Kupon kupon = ulubionaKolektura.wydajKupon(this, 1, 1);
        if(kupon != null) super.dodajKupon(kupon);
    }

}
