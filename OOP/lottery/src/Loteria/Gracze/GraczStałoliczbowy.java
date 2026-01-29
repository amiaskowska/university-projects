package Loteria.Gracze;

import Loteria.Blankiet;
import Loteria.Centrala;
import Loteria.Kolektura;
import Loteria.Kupon;

import java.util.ArrayList;

public class GraczStałoliczbowy extends Gracz {
    private int[] ulubioneLiczby;
    private ArrayList<Kolektura> ulubioneKolektury;
    private int obecnaKolektura;

    public GraczStałoliczbowy(String imię, String nazwisko, String PESEL, long złote,
                              long grosze, Centrala centrala, int[] ulubioneLiczby,
                              Kolektura[] ulubioneKolektury) {
        super(imię, nazwisko, PESEL, złote, grosze, centrala);
        this.ulubioneLiczby = new int[6];
        for (int i = 0; i < 6; ++i)
            this.ulubioneLiczby[i] = ulubioneLiczby[i];
        this.ulubioneKolektury = new ArrayList<>();
        for(int i = 0; i < ulubioneKolektury.length; ++i)
            this.ulubioneKolektury.add(ulubioneKolektury[i]);
        obecnaKolektura = 0;
    }

    @Override
    public void kupKupon(){
        Kupon ostatniKupon = super.getKupon(super.getLiczbaKuponów() - 1);
        if(ostatniKupon != null) {
            int pierwszeLosowanie = ostatniKupon.getPierwszeLosowanie();
            int liczbaLosowań = ostatniKupon.getLiczbaLosowań();
            int następneLosowanie = super.getCentrala().getNumerLosowania();
            if ((pierwszeLosowanie + liczbaLosowań) <= następneLosowanie);
        }

        Blankiet blankiet = new Blankiet();
        for(int i = 0; i < 6; ++i){
            blankiet.skreśl(ulubioneLiczby[i], 1);
        }
        blankiet.wybierzLiczbęLosowań(10);
        Kolektura kolektura = ulubioneKolektury.get(obecnaKolektura);
        obecnaKolektura = (obecnaKolektura + 1) % ulubioneKolektury.size();
        Kupon kupon = kolektura.wydajKupon(this, blankiet);
        if(kupon != null) super.dodajKupon(kupon);
    }
}
