
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, random_split
from torchvision import datasets, transforms
import matplotlib.pyplot as plt
import numpy as np
import torch
import copy

# Device (GPU si dispo sur Colab)
DEVICE = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
print("Device utilisé :", DEVICE)

def get_mnist_loaders(val_ratio=0.1, batch_size=128, data_root="./data"):
    """
    Télécharge MNIST et renvoie les DataLoaders train/val/test.
    - val_ratio : proportion du train utilisée pour validation
    - batch_size : taille de batch
    - data_root  : dossier où stocker les données
    """
    # Transformations : normalisation + passage en tenseur
    transform = transforms.Compose([
        transforms.ToTensor(),               # [0,255] -> [0.0,1.0]
    ])

    # Dataset complet d'entraînement
    full_train_dataset = datasets.MNIST(
        root=data_root,
        train=True,
        download=True,
        transform=transform
    )

    # Split train / validation
    n_total = len(full_train_dataset)
    n_val = int(n_total * val_ratio)
    n_train = n_total - n_val

    train_dataset, val_dataset = random_split(
        full_train_dataset,
        [n_train, n_val],
        generator=torch.Generator().manual_seed(42)  # pour reproductibilité
    )

    # Dataset de test
    test_dataset = datasets.MNIST(
        root=data_root,
        train=False,
        download=True,
        transform=transform
    )

    # DataLoaders
    train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
    val_loader   = DataLoader(val_dataset,   batch_size=batch_size, shuffle=False)
    test_loader  = DataLoader(test_dataset,  batch_size=batch_size, shuffle=False)

    print(f"Train : {len(train_dataset)} imgs, Val : {len(val_dataset)} imgs, Test : {len(test_dataset)} imgs")

    return train_loader, val_loader, test_loader

train_loader, val_loader, test_loader = get_mnist_loaders(
    val_ratio=0.1,
    batch_size=128
)

class VanillaAutoencoder(nn.Module):
    def __init__(self, input_dim=28*28, hidden_dims=[256, 128], latent_dim=32):
        """
        input_dim   : dimension d'entrée (784 pour MNIST 28x28)
        hidden_dims : liste des tailles couches cachées encodeur/decodeur
        latent_dim  : dimension de l'espace latent
        """
        super().__init__()

        # ---- Encodeur ----
        encoder_layers = []
        prev_dim = input_dim
        for h_dim in hidden_dims:
            encoder_layers.append(nn.Linear(prev_dim, h_dim))
            encoder_layers.append(nn.ReLU())
            prev_dim = h_dim
        # dernière couche de l'encodeur -> espace latent
        encoder_layers.append(nn.Linear(prev_dim, latent_dim))

        self.encoder = nn.Sequential(*encoder_layers)

        # ---- Décodeur ----
        decoder_layers = []
        prev_dim = latent_dim
        # on reverse les hidden_dims pour avoir un décodeur "miroir"
        for h_dim in reversed(hidden_dims):
            decoder_layers.append(nn.Linear(prev_dim, h_dim))
            decoder_layers.append(nn.ReLU())
            prev_dim = h_dim
        # dernière couche du décodeur -> sortie (même taille que l'entrée)
        decoder_layers.append(nn.Linear(prev_dim, input_dim))
        decoder_layers.append(nn.Sigmoid())  # pour obtenir des pixels dans [0,1]

        self.decoder = nn.Sequential(*decoder_layers)

    def forward(self, x):
        """
        x : [B, 1, 28, 28] ou [B, 784]
        """
        # Si l'image est [B,1,28,28], on l'applatit
        if x.dim() == 4 and x.shape[1:] == (1, 28, 28):
            B = x.size(0)
            x = x.view(B, -1)  # [B, 784]

        z = self.encoder(x)     # [B, latent_dim]
        x_recon = self.decoder(z)  # [B, 784]

        # Remettre en format image si besoin
        # Si on veut [B,1,28,28] en sortie :
        # x_recon = x_recon.view(-1, 1, 28, 28)

        return x_recon, z

# =====================================================================
# FONCTION DE RECHERCHE D'HYPERPARAMÈTRES (MSE UNIQUEMENT)
# =====================================================================
def sae_hyperparameter_search(train_loader, val_loader, lambda_values, num_epochs=20):
    """
    Entraîne des SAE plats et sur-complets (784 -> 1024 -> 784) avec injection de ReLU.
    Traque l'évolution du MSE pour chaque Lambda.
    """
    results_histories = {}
    final_val_losses = []
    models_saved = {}

    for l_val in lambda_values:
        print(f"\n--- Entraînement SAE (Plat - Overcomplete) | Lambda = {l_val} ---")


        model = VanillaAutoencoder(input_dim=784, hidden_dims=[], latent_dim=1024).to(DEVICE)

        # On injecte l'activation ReLU à la fin de l'encodeur
        # pour forcer la création de vrais zéros absolus sous la contrainte L1
        model.encoder.add_module("sparsity_relu", nn.ReLU())

        criterion = nn.MSELoss()
        optimizer = optim.Adam(model.parameters(), lr=1e-3)

        history = {"train_loss": [], "val_loss": []}

        for epoch in range(num_epochs):
            # --- Entraînement ---
            model.train()
            train_loss = 0.0
            for batch_x, _ in train_loader:
                batch_x = batch_x.to(DEVICE)
                batch_x_flat = batch_x.view(batch_x.size(0), -1)

                optimizer.zero_grad()
                x_recon, z = model(batch_x)

                # Perte globale = Erreur de reconstruction (MSE) + Pénalité L1 (Sparsité)
                mse = criterion(x_recon, batch_x_flat)
                l1_penalty = torch.mean(torch.abs(z))
                loss = mse + l_val * l1_penalty

                loss.backward()
                optimizer.step()
                train_loss += mse.item() * batch_x.size(0)

            train_loss /= len(train_loader.dataset)
            history["train_loss"].append(train_loss)

            # --- Validation ---
            model.eval()
            val_loss = 0.0
            with torch.no_grad():
                for batch_x, _ in val_loader:
                    batch_x = batch_x.to(DEVICE)
                    batch_x_flat = batch_x.view(batch_x.size(0), -1)
                    x_recon, z = model(batch_x)
                    mse = criterion(x_recon, batch_x_flat)
                    val_loss += mse.item() * batch_x.size(0)

            val_loss /= len(val_loader.dataset)
            history["val_loss"].append(val_loss)

        results_histories[l_val] = history
        final_val_losses.append(history["val_loss"][-1])
        models_saved[l_val] = copy.deepcopy(model)

        print(f"MSE Final (Validation) : {history['val_loss'][-1]:.5f}")

    return results_histories, final_val_losses, models_saved

# =====================================================================
# 1. LANCEMENT DE L'EXPÉRIENCE AVEC LES 5 LAMBDAS
# =====================================================================
lambdas_to_test = [0.0, 1e-4, 1e-3, 1e-2, 5e-2]
sae_histories, sae_final_mse, sae_models = sae_hyperparameter_search(
    train_loader, val_loader, lambdas_to_test, num_epochs=20
)

# =====================================================================
# 2. AFFICHAGE ET SAUVEGARDE SÉPARÉE DES GRAPHES BILANS (FORMAT EPS)
# =====================================================================
lambdas_str = [str(l) for l in lambdas_to_test]

# --- GRAPHQUE 1 : Évolution du MSE au cours de l'entrainement (Convergence) ---
plt.figure(figsize=(8, 6))
for l_val in lambdas_to_test:
    plt.plot(sae_histories[l_val]['val_loss'], label=fr'$\lambda$={l_val}')
plt.title('Évolution du MSE (Validation) au cours des époques', fontsize=12)
plt.xlabel('Epochs', fontsize=11)
plt.ylabel('MSE', fontsize=11)
plt.legend()
plt.grid(True, linestyle='--', alpha=0.5)
plt.tight_layout()
plt.savefig('sae_1_convergence_mse.eps', format='eps', bbox_inches='tight')
plt.show()

# --- GRAPHQUE 2 : La Courbe en U (Impact de Lambda sur le MSE final) ---
plt.figure(figsize=(8, 6))
plt.plot(lambdas_str, sae_final_mse, marker='o', color='red', linewidth=2)
plt.title(r'Impact de $\lambda$ sur l\'erreur de reconstruction finale', fontsize=12)
plt.xlabel(r'Valeur de $\lambda$ (Pénalité $L_1$)', fontsize=11)
plt.ylabel('MSE Final sur Validation', fontsize=11)
plt.grid(True, linestyle='--', alpha=0.5)
plt.tight_layout()
plt.savefig('sae_2_ucurve_impact.eps', format='eps', bbox_inches='tight')
plt.show()

# =====================================================================
# 3. VISUALISATION EN LIGNE DES RECONSTRUCTIONS POUR LES  5 LAMBDAS
# =====================================================================
def visualize_single_image_sae_grid(models_dict, test_loader, lambdas):
    dataiter = iter(test_loader)
    images, _ = next(dataiter)
    single_image = images[3:4].to(DEVICE)

    num_cols = len(lambdas) + 1
    fig, axes = plt.subplots(nrows=1, ncols=num_cols, figsize=(num_cols * 3.2, 3.5))
    fig.suptitle(r"Impact de la pénalité $L_1$ sur un SAE plat sur-complet ($784 \rightarrow 1024 \rightarrow 784$)", fontsize=13, y=1.05)

    # Image originale (index 0)
    axes[0].imshow(single_image.cpu().squeeze(), cmap='gray')
    axes[0].set_title("Original", fontweight='bold')
    axes[0].axis('off')

    # Reconstructions pour chaque modèle
    for idx, l_val in enumerate(lambdas):
        model = models_dict[l_val]
        model.eval()
        with torch.no_grad():
            reconstructed, _ = model(single_image)
            reconstructed = reconstructed.view(28, 28)

        ax = axes[idx + 1]
        ax.imshow(reconstructed.cpu(), cmap='gray')

        if l_val == 0.0:
            ax.set_title(r"Vanilla ($\lambda=0$)" + "\n(Mémorisation totale)")
        elif l_val == lambdas[-1]:
            ax.set_title(fr"Extrême ($\lambda={l_val}$)" + "\n(Trop éteint)")
        else:
            ax.set_title(fr"SAE ($\lambda={l_val}$)")
        ax.axis('off')

    plt.tight_layout()
    plt.savefig('sae_3_grid_reconstructions.eps', format='eps', bbox_inches='tight')
    plt.show()

visualize_single_image_sae_grid(sae_models, test_loader, lambdas_to_test)

# =====================================================================
# 4. ÉTUDE DE SPARSITÉ : ACTIVATIONS MOYENNES POUR LES  5 LAMBDAS
# =====================================================================
def analyse_overall_sparsity_grid(models_dict, test_loader, lambdas_to_plot):
    """
    Passe tout le dataset de test pour CHAQUE Lambda, accumule les activations,
    et affiche la grille complète des diagrammes de barres pour les 1024 neurones.
    """
    fig, axes = plt.subplots(nrows=len(lambdas_to_plot), ncols=1, figsize=(16, 3.2 * len(lambdas_to_plot)), sharex=True)
    if len(lambdas_to_plot) == 1:
        axes = [axes]

    fig.suptitle("Profil d'activation moyen des 1024 neurones sur l'intégralité du dataset de test (MNIST)", fontsize=14, y=1.02)

    for idx, l_val in enumerate(lambdas_to_plot):
        model = models_dict[l_val]
        model.eval()

        all_z = []
        with torch.no_grad():
            for batch_x, _ in test_loader:
                batch_x = batch_x.to(DEVICE)
                _, z = model(batch_x)
                all_z.append(z.cpu())

        all_z = torch.cat(all_z, dim=0)
        avg_activations = torch.mean(all_z, dim=0).numpy()

        ax = axes[idx]
        ax.bar(range(1024), avg_activations, color='royalblue', width=1.0, alpha=0.9)

        ax.set_title(fr"Taux d'activation moyen pour Lambda = {l_val}", fontweight='bold', fontsize=11)
        ax.set_ylabel("Activation Moyenne")
        ax.grid(True, linestyle='--', alpha=0.4)

    axes[-1].set_xlabel("Index du neurone latent (0 à 1023)")
    plt.tight_layout()
    plt.savefig('sae_4_sparsity_neuron_profiles.eps', format='eps', bbox_inches='tight')
    plt.show()

# Lancement de l'analyse exhaustive sur des 5  valeurs
analyse_overall_sparsity_grid(sae_models, test_loader, lambdas_to_test)