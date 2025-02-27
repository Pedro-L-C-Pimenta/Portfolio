package projeto322;

import java.time.LocalDate;

import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

public class Main extends Application {

    private Ingresso ingressoSelecionado;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Compra de Ingressos");

        // Criação dos escudos genéricos dos times
        Image escudo1 = new Image(getClass().getResourceAsStream("/resources/escudo1.png"));
        Image escudo2 = new Image(getClass().getResourceAsStream("/resources/escudo2.png"));
        Image escudo3 = new Image(getClass().getResourceAsStream("/resources/escudo3.png"));
        Image escudo4 = new Image(getClass().getResourceAsStream("/resources/escudo4.png"));

        // Criação dos rótulos dos times
        Label time1Label = new Label("Time 1");
        Label time2Label = new Label("Time 2");
        Label time3Label = new Label("Time 3");
        Label time4Label = new Label("Time 4");

        // Criação dos botões para escolha dos jogos
        Button jogo1Button = createGameButton(escudo1, time1Label, time2Label, new Partida());
        Button jogo2Button = createGameButton(escudo1, time3Label, time4Label, new Partida());
        Button jogo3Button = createGameButton(escudo2, time1Label, time3Label, new Partida());
        Button jogo4Button = createGameButton(escudo2, time2Label, time4Label, new Partida());

        // Criação do ComboBox para seleção do tipo de ingresso
        ComboBox<String> ingressoComboBox = new ComboBox<>();
        ingressoComboBox.getItems().addAll("Inteira", "Meia-entrada");
        ingressoComboBox.getSelectionModel().selectFirst();

        // Criação do botão de pagamento
        Button pagarButton = new Button("Pagar");
        pagarButton.setOnAction(e -> {
            if (ingressoSelecionado != null) {
                try {
                    ingressoSelecionado.pagar();
                    System.out.println("Pagamento realizado com sucesso!");
                } catch (IllegalArgumentException ex) {
                    System.out.println("Erro ao realizar o pagamento: " + ex.getMessage());
                }
            } else {
                System.out.println("Selecione um ingresso antes de prosseguir com o pagamento.");
            }
        });

        // Criação do layout principal
        VBox layout = new VBox(10);
        layout.setPadding(new Insets(20));
        layout.setAlignment(Pos.CENTER);

        // Adicionando os componentes ao layout principal
        layout.getChildren().addAll(jogo1Button, jogo2Button, jogo3Button, jogo4Button, ingressoComboBox, pagarButton);

        // Criação da cena principal
        Scene scene = new Scene(layout, 400, 400);
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    // Método auxiliar para criar os botões dos jogos
    private Button createGameButton(Image escudo, Label time1Label, Label time2Label, Partida partida) {
        Button button = new Button();
        ImageView imageView = new ImageView(escudo);
        imageView.setFitWidth(50);
        imageView.setFitHeight(50);
        button.setGraphic(imageView);

        VBox vBox = new VBox(5);
        vBox.setAlignment(Pos.CENTER);
        vBox.getChildren().addAll(time1Label, time2Label);

        BorderPane borderPane = new BorderPane();
        borderPane.setCenter(vBox);
        button.setGraphic(borderPane);

        button.setOnAction(e -> {
            GUIestádio guIestádio = new GUIestádio();
            String nome_setor = guIestádio.retornaSetor();
            Setor setor = new Setor(nome_setor, 10000); // Crie uma instância apropriada do Setor
            int assento = 1; // Defina o assento apropriado
            Double preco = 50.0; // Defina o preço apropriado

            try {
                ingressoSelecionado = new Ingresso(preco, setor, assento, LocalDate.now().plusDays(1), partida);
                System.out.println("Ingresso selecionado: " + partida.toString());
            } catch (IllegalArgumentException ex) {
                System.out.println("Erro ao selecionar o ingresso: " + ex.getMessage());
                ingressoSelecionado = null;
            }
        });

        return button;
    }
}