
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, random_split
from torchvision import datasets, transforms
import matplotlib.pyplot as plt
import numpy as np
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
# 1. ENTRAÎNEMENT DU DENOISING AUTOENCODER (DAE)
# =====================================================================
def dae_hyperparameter_search(train_loader, val_loader, noise_levels, num_epochs=20):
    """
    Entraîne des Autoencodeurs face à différents niveaux de bruit gaussien.
    Calcule le MSE de reconstruction final par rapport aux images PROPRES.
    """
    results_histories = {}
    final_val_losses = []
    models_saved = {}

    for sigma in noise_levels:
        print(f"\n--- Entraînement DAE | Niveau de Bruit (Sigma) = {sigma} ---")
        model = VanillaAutoencoder(input_dim=784, hidden_dims=[], latent_dim=1024).to(DEVICE)
        criterion = nn.MSELoss()
        optimizer = optim.Adam(model.parameters(), lr=1e-3)

        history = {"train_loss": [], "val_loss": []}

        for epoch in range(num_epochs):
            model.train()
            train_loss = 0.0
            for batch_x, _ in train_loader:
                batch_x = batch_x.to(DEVICE)
                batch_x_flat = batch_x.view(batch_x.size(0), -1)

                # INJECTION DU BRUIT GAUSSIEN UNIQUEMENT SUR L'ENTRÉE
                noise = sigma * torch.randn_like(batch_x_flat)
                noisy_batch_x = batch_x_flat + noise
                noisy_batch_x = torch.clamp(noisy_batch_x, 0.0, 1.0) # On reste entre 0 et 1

                optimizer.zero_grad()
                x_recon, _ = model(noisy_batch_x)

                # CRUCIAL : La perte compare la reconstruction avec l'image PROPRE d'origine
                loss = criterion(x_recon, batch_x_flat)

                loss.backward()
                optimizer.step()
                train_loss += loss.item() * batch_x.size(0)

            train_loss /= len(train_loader.dataset)
            history["train_loss"].append(train_loss)

            # --- Validation ---
            model.eval()
            val_loss = 0.0
            with torch.no_grad():
                for batch_x, _ in val_loader:
                    batch_x = batch_x.to(DEVICE)
                    batch_x_flat = batch_x.view(batch_x.size(0), -1)

                    # Même bruit appliqué en validation
                    noise = sigma * torch.randn_like(batch_x_flat)
                    noisy_batch_x = torch.clamp(batch_x_flat + noise, 0.0, 1.0)

                    x_recon, _ = model(noisy_batch_x)
                    loss = criterion(x_recon, batch_x_flat)
                    val_loss += loss.item() * batch_x.size(0)

            val_loss /= len(val_loader.dataset)
            history["val_loss"].append(val_loss)

        results_histories[sigma] = history
        final_val_losses.append(history["val_loss"][-1])
        models_saved[sigma] = copy.deepcopy(model)
        print(f"MSE Final (Validation) : {history['val_loss'][-1]:.5f}")

    return results_histories, final_val_losses, models_saved

# =====================================================================
# 2. LANCEMENT DE L'EXPÉRIENCE DAE
# =====================================================================
noise_levels_to_test = [0.0, 0.1, 0.3, 0.5]
dae_histories, dae_final_mse, dae_models = dae_hyperparameter_search(
    train_loader, val_loader, noise_levels_to_test, num_epochs=20
)

# =====================================================================
# 3. TRACÉ ET SAUVEGARDE DES GRAPHES (SÉPARÉS ET SANS WARNINGS)
# =====================================================================
noise_str = [str(s) for s in noise_levels_to_test]

# --- GRAPHQUE 1 : Convergence du MSE ---
plt.figure(figsize=(8, 5))
for sigma in noise_levels_to_test:
    plt.plot(dae_histories[sigma]['val_loss'], label=fr'$\sigma$={sigma}')
plt.title('DAE : Évolution du MSE de Validation au cours des époques')
plt.xlabel('Epochs')
plt.ylabel('MSE (Calculé sur image propre)')
plt.legend()
plt.grid(True, linestyle='--', color='lightgray')
plt.tight_layout()
plt.savefig('dae_1_convergence.eps', format='eps', bbox_inches='tight')
plt.show()

# --- GRAPHQUE 2 : Courbe d'efficacité (DAE vs Bruit d'entrée de référence) ---
# Calcul de la référence mathématique du bruit d'entrée réel injecté
input_noise_mse = [s**2 if s > 0 else 0.0 for s in noise_levels_to_test]

plt.figure(figsize=(8, 5))
plt.plot(noise_str, input_noise_mse, marker='x', linestyle='--', color='black', label="Image Bruitée d'entrée")
plt.plot(noise_str, dae_final_mse, marker='o', linestyle='-', color='crimson', linewidth=2, label="Image Débruitée (DAE)")
plt.title("Efficacité du DAE : Erreur d'entrée vs Erreur de sortie")
plt.xlabel(fr'Intensité du bruit gaussien ($\sigma$)')
plt.ylabel('MSE Final')
plt.legend()
plt.grid(True, linestyle='--', color='lightgray')
plt.tight_layout()
plt.savefig('dae_2_efficacite.eps', format='eps', bbox_inches='tight')
plt.show()

# =====================================================================
# 4. TRACÉ DE LA GRILLE VISUELLE (ORIGINAL -> BRUITÉ -> RECONSTRUIT)
# =====================================================================
def visualize_dae_matrix(models_dict, test_loader, noise_levels):
    dataiter = iter(test_loader)
    images, _ = next(dataiter)
    single_image = images[3:4].to(DEVICE)
    single_image_flat = single_image.view(1, -1)

    num_cols = len(noise_levels)
    fig, axes = plt.subplots(nrows=2, ncols=num_cols, figsize=(num_cols * 3, 6))
    fig.suptitle("Visualisation qualitative du nettoyage d'image par le DAE", fontsize=12, y=1.02)

    for idx, sigma in enumerate(noise_levels):
        # 1. Génération du bruit
        noise = sigma * torch.randn_like(single_image_flat)
        noisy_img = torch.clamp(single_image_flat + noise, 0.0, 1.0).view(28, 28)

        # Ligne 1 : Image bruitée en entrée
        axes[0, idx].imshow(noisy_img.cpu().numpy(), cmap='gray')
        axes[0, idx].set_title(fr"Entrée ($\sigma={sigma}$)")
        axes[0, idx].axis('off')

        # 2. Passage dans le modèle DAE correspondant
        model = models_dict[sigma]
        model.eval()
        with torch.no_grad():
            reconstructed, _ = model(noisy_img.view(1, -1).to(DEVICE))
            reconstructed = reconstructed.view(28, 28)

        # Ligne 2 : Sortie nettoyée par le modèle
        axes[1, idx].imshow(reconstructed.cpu().numpy(), cmap='gray')
        axes[1, idx].set_title(fr"Sortie DAE")
        axes[1, idx].axis('off')

    plt.tight_layout()
    plt.savefig('dae_3_grille_visuelle.eps', format='eps', bbox_inches='tight')
    plt.show()

visualize_dae_matrix(dae_models, test_loader, noise_levels_to_test)