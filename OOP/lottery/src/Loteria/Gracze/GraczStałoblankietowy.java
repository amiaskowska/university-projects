package Loteria.Gracze;

import Loteria.Blankiet;
import Loteria.Centrala;
import Loteria.Kolektura;
import Loteria.Kupon;

import java.util.ArrayList;

public class GraczStałoblankietowy extends Gracz {
    private Blankiet ulubionyBlankiet;
    private ArrayList<Kolektura> ulubioneKolektury;
    private int obecnaKolektura;

    public GraczStałoblankietowy(String imię, String nazwisko, String PESEL,
                                 long złote, long grosze, Centrala centrala, Blankiet ulubionyBlankiet,
                                 Kolektura[] ulubioneKolektury) {
        super(imię, nazwisko, PESEL, złote, grosze, centrala);
        this.ulubionyBlankiet = ulubionyBlankiet.kopia();
        this.ulubioneKolektury = new ArrayList<>();
        for (int i = 0; i < ulubioneKolektury.length; ++i)
            this.ulubioneKolektury.add(ulubioneKolektury[i]);
        obecnaKolektura = 0;
    }

    @Override
    public void kupKupon(){
        Kolektura kolektura = ulubioneKolektury.get(obecnaKolektura);
        obecnaKolektura = (obecnaKolektura + 1) % ulubioneKolektury.size();
        Kupon kupon = kolektura.wydajKupon(this, ulubionyBlankiet);
        if(kupon != null) super.dodajKupon(kupon);
    }
}
