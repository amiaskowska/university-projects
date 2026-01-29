package Loteria.Gracze;

import Loteria.Centrala;
import Loteria.Kolektura;
import Loteria.Kupon;
import Loteria.MaszynaLosująca;

public class GraczLosowy extends Gracz {
    private MaszynaLosująca rand = new MaszynaLosująca();
    public GraczLosowy(String imię, String nazwisko, String PESEL, long złote, long grosze, Centrala centrala){
        super(imię, nazwisko, PESEL, złote, grosze, centrala);

    }

    public void nowaKolektura(Kolektura kolektura){
        rand.nowaKolektura(kolektura);
    }

    @Override
    public void kupKupon() {
        Kolektura kolektura = rand.losowaKolektura();
        int liczbaKuponów = rand.losowaLiczbaKuponów();
        for(int i = 0; i < liczbaKuponów; ++i) {
            int liczbaLosowań = rand.losowaLiczbaLosowań();
            int liczbaZakładów = rand.losowaLiczbaZakładów();
            Kupon kupon = kolektura.wydajKupon(this, liczbaZakładów, liczbaLosowań);
            if (kupon != null) super.dodajKupon(kupon);
        }
    }
}
