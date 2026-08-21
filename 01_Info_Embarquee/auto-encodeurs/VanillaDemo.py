import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, random_split
from torchvision import datasets, transforms
import matplotlib.pyplot as plt
import numpy as np

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

model = VanillaAutoencoder(
    input_dim=28*28,
    hidden_dims=[256, 128],
    latent_dim=32
).to(DEVICE)

print(model)

def training_loop(
    model,
    train_loader,
    val_loader,
    num_epochs=20,
    learning_rate=1e-3,
    weight_decay=1e-5,
    device=DEVICE,
    verbose=True
):
    """
    Boucle d'entraînement standard pour auto-encodeur (MSE).
    Renvoie un dictionnaire avec les losses train/val par epoch.
    """
    criterion = nn.MSELoss()
    optimizer = optim.Adam(
        model.parameters(),
        lr=learning_rate,
        weight_decay=weight_decay
    )

    history = {
        "train_loss": [],
        "val_loss": []
    }

    for epoch in range(num_epochs):
        # ---- Phase train ----
        model.train()
        train_loss = 0.0
        n_train = 0

        for batch_x, _ in train_loader:  # labels non utilisés
            batch_x = batch_x.to(device)
            batch_x_flattened = batch_x.view(batch_x.size(0), -1)


            # Forward
            x_recon, _ = model(batch_x)
            loss = criterion(x_recon, batch_x_flattened)

            # Backward
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

            # Accumulation de la loss
            train_loss += loss.item() * batch_x.size(0)
            n_train += batch_x.size(0)

        train_loss /= n_train
        history["train_loss"].append(train_loss)

        # ---- Phase validation ----
        model.eval()
        val_loss = 0.0
        n_val = 0

        with torch.no_grad():
            for batch_x, _ in val_loader:
                batch_x = batch_x.to(device)
                batch_x_flattened = batch_x.view(batch_x.size(0), -1)

                x_recon, _ = model(batch_x)
                loss = criterion(x_recon, batch_x_flattened)

                val_loss += loss.item() * batch_x.size(0)
                n_val += batch_x.size(0)

        val_loss /= n_val
        history["val_loss"].append(val_loss)

        if verbose:
            print(f"Epoch [{epoch+1}/{num_epochs}] "
                  f"Train Loss: {train_loss:.6f} | "
                  f"Val Loss: {val_loss:.6f}")

    return history

# 1) DataLoaders
train_loader, val_loader, test_loader = get_mnist_loaders(
    val_ratio=0.1,
    batch_size=128
)

# 2) Modèle
model = VanillaAutoencoder(
    input_dim=28*28,
    hidden_dims=[256, 128],
    latent_dim=32
).to(DEVICE)

# 3) Entraînement
history = training_loop(
    model,
    train_loader,
    val_loader,
    num_epochs=20,
    learning_rate=1e-3,
    weight_decay=1e-5,
    device=DEVICE,
    verbose=True
)

import matplotlib.pyplot as plt
import numpy as np
import torch # Import torch

# Define DEVICE to ensure it's available in this cell's scope
DEVICE = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

def visualiser_etapes(model, test_loader, device=DEVICE, n_images=5):
    """
    Affiche côte à côte : l'image originale, l'espace latent (reshape en 4x8), et l'image reconstruite.
    """
    # On prend un batch d'images de test
    images, _ = next(iter(test_loader))
    images = images[:n_images].to(device) # Use the function's device argument

    # On passe les images dans le modèle (sans calculer les gradients car on est en mode évaluation)
    model.eval()
    with torch.no_grad():
        reconstructions, latent_vectors = model(images)

    # On remet les tenseurs sur CPU et on les convertit en numpy pour matplotlib
    images = images.cpu().numpy()
    latent_vectors = latent_vectors.cpu().numpy()
    reconstructions = reconstructions.cpu().numpy()

    # Création de la figure (n_images lignes, 3 colonnes)
    fig, axes = plt.subplots(n_images, 3, figsize=(10, n_images * 2.5))

    # Titres des colonnes
    colonnes = ["1. Entrée (Original)", "2. Espace Latent (Résumé)", "3. Sortie (Reconstruction)"]
    for ax, col in zip(axes[0], colonnes):
        ax.set_title(col, fontsize=12, fontweight='bold')

    for i in range(n_images):
        # --- Colonne 1 : Image Originale ---
        # L'image est de forme (1, 28, 28), on enlève la dimension du canal pour matplotlib
        img_orig = images[i].squeeze()
        axes[i, 0].imshow(img_orig, cmap='gray')
        axes[i, 0].axis('off')

        # --- Colonne 2 : Espace Latent ---
        # Le vecteur latent est de taille 32. On le reshape en (4, 8) pour pouvoir l'afficher
        img_latent = latent_vectors[i].reshape(4, 8)
        # On utilise une carte de chaleur (heatmap) pour voir l'activation des neurones
        axes[i, 1].imshow(img_latent, cmap='viridis', aspect='auto')
        axes[i, 1].axis('off')
        # On ajoute le texte "32 dims" pour bien expliquer
        axes[i, 1].set_xlabel('32 dims', fontsize=9, color='gray')

        # --- Colonne 3 : Image Reconstruite ---
        # La reconstruction est de taille 784, on la reshape en (28, 28)
        img_recon = reconstructions[i].reshape(28, 28)
        axes[i, 2].imshow(img_recon, cmap='gray')
        axes[i, 2].axis('off')

    plt.tight_layout()
    plt.show()

# --- EXÉCUTION DE LA VISUALISATION ---
# Assure-toi que ton modèle 'model' est bien entraîné avant de lancer ça !
visualiser_etapes(model, test_loader, n_images=5)