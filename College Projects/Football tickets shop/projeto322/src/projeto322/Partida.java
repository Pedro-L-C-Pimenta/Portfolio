package projeto322;

import java.util.*;
import java.sql.Time;
import java.time.LocalTime;

public class Partida {
    private Date data;
    private String estádio;
    private LocalTime horário;
    private Clube mandante;
    private Clube visitante;

    //Construtor
    public Partida(Date data, LocalTime horário, Clube mandante, Clube visitante) {
        this.data = data;
        this.horário = horário;
        this.mandante = mandante;
        this.estádio = mandante.getEstádio();
        this.visitante = visitante;
    }
    public Partida() {
    }
    //Getters e Setters
    public Date getData() {
        return data;
    }
    public void setData(Date data) {
        this.data = data;
    }

    public String getEstádio() {
        return estádio;
    }
    public void setEstádio(String estádio) {
        this.estádio = estádio;
    }

    public LocalTime getHorário() {
        return horário;
    }
    public void setHorário(LocalTime horário) {
        this.horário = horário;
    }

    public Clube getMandante() {
        return mandante;
    }
    public void setMandante(Clube mandante) {
        this.mandante = mandante;
    }

    public Clube getVisitante() {
        return visitante;
    }
    public void setVisitante(Clube visitante) {
        this.visitante = visitante;
    }

    //Alteração de Data e/ou horário
    public void adiaPartida(Date novaData) {
        setData(novaData);
    }
    public void adiaPartida(LocalTime novoHorário) {
        setHorário(novoHorário);
    }
    public void adiaPartida(Date novaData, LocalTime novoHorário) {
        setData(novaData);
        setHorário(novoHorário);
    }
    


}