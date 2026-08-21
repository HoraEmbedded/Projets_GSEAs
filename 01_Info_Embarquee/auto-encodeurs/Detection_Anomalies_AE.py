import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms
from torch.utils.data import DataLoader, Subset, random_split
import matplotlib.pyplot as plt
import numpy as np

# Données issues du modèles Vanilla Demo précédente

# Définition du device (GPU si disponible)
DEVICE = torch.device("cuda" if torch.cuda.is_available() else "cpu")
BATCH_SIZE = 128

transform = transforms.Compose([transforms.ToTensor()])

# Téléchargement et préparation du dataset MNIST complet
dataset_complet = datasets.MNIST(root='./data', train=True, transform=transform, download=True)
test_dataset = datasets.MNIST(root='./data', train=False, transform=transform, download=True)

train_size = int(0.8 * len(dataset_complet))
val_size = len(dataset_complet) - train_size
train_dataset, val_dataset = random_split(dataset_complet, [train_size, val_size])

# Loaders standards contenant toutes les classes (0 à 9)
train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True)
val_loader = DataLoader(val_dataset, batch_size=BATCH_SIZE, shuffle=False)
test_loader = DataLoader(test_dataset, batch_size=BATCH_SIZE, shuffle=False)

# Architecture du Vanilla Auto-encodeur
class VanillaAutoencoder(nn.Module):
    def __init__(self, latent_dim=32):
        super(VanillaAutoencoder, self).__init__()
        self.encoder = nn.Sequential(
            nn.Linear(28 * 28, 128), nn.ReLU(),
            nn.Linear(128, 64), nn.ReLU(),
            nn.Linear(64, latent_dim), nn.ReLU()
        )
        self.decoder = nn.Sequential(
            nn.Linear(latent_dim, 64), nn.ReLU(),
            nn.Linear(64, 128), nn.ReLU(),
            nn.Linear(128, 28 * 28), nn.Sigmoid()
        )

    def forward(self, x):
        x = x.view(x.size(0), -1)
        latent = self.encoder(x)
        reconstructed = self.decoder(latent)
        return reconstructed, latent

# Boucle d'entraînement globale
def training_loop(model, train_loader, val_loader, num_epochs, learning_rate, weight_decay=0, device='cpu', verbose=True):
    criterion = nn.MSELoss()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate, weight_decay=weight_decay)
    history = {'train_loss': [], 'val_loss': []}
    
    for epoch in range(num_epochs):
        model.train()
        train_loss = 0.0
        for images, _ in train_loader:
            images = images.to(device)
            optimizer.zero_grad()
            outputs, _ = model(images)
            loss = criterion(outputs, images.view(images.size(0), -1))
            loss.backward()
            optimizer.step()
            train_loss += loss.item() * images.size(0)
            
        train_loss /= len(train_loader.dataset)
        history['train_loss'].append(train_loss)
        
        model.eval()
        val_loss = 0.0
        with torch.no_grad():
            for images, _ in val_loader:
                images = images.to(device)
                outputs, _ = model(images)
                loss = criterion(outputs, images.view(images.size(0), -1))
                val_loss += loss.item() * images.size(0)
                
        val_loss /= len(val_loader.dataset)
        history['val_loss'].append(val_loss)
        
        if verbose and (epoch + 1) % 5 == 0:
            print(f"Époque [{epoch+1}/{num_epochs}] - Train Loss: {train_loss:.4f} | Val Loss: {val_loss:.4f}")
            
    return history



# PARTIE 2 : DÉTECTION D'ANOMALIES & BILAN 


# --- 1. Filtrage des données et exclusion de l'anomalie ---
CLASSE_ANOMALIE = 9       

# Extraction et isolation de la classe pathologique
train_filtered_idx = [i for i, (_, label) in enumerate(train_loader.dataset.dataset) if label != CLASSE_ANOMALIE]
train_anom_dataset = Subset(train_loader.dataset.dataset, train_filtered_idx)

val_filtered_idx = [i for i, (_, label) in enumerate(val_loader.dataset.dataset) if label != CLASSE_ANOMALIE]
val_anom_dataset = Subset(val_loader.dataset.dataset, val_filtered_idx)

anomaly_train_loader = DataLoader(train_anom_dataset, batch_size=BATCH_SIZE, shuffle=True)
anomaly_val_loader   = DataLoader(val_anom_dataset,   batch_size=BATCH_SIZE, shuffle=False)
anomaly_test_loader  = DataLoader(test_loader.dataset, batch_size=BATCH_SIZE, shuffle=False)

# --- 2. Entraînement du modèle dédié (Données saines uniquement) ---
print("\n=== Entraînement du modèle de détection d'anomalies ===")
model_detection = VanillaAutoencoder(latent_dim=32).to(DEVICE)
history_anom = training_loop(
    model=model_detection,
    train_loader=anomaly_train_loader,
    val_loader=anomaly_val_loader,
    num_epochs=20,
    learning_rate=1e-3,
    weight_decay=1e-5,
    device=DEVICE,
    verbose=True
)

# --- 3. Inférence et calcul de l'erreur quadratique moyenne (MSE) ---
model_detection.eval()
all_errors, all_labels = [], []

with torch.no_grad():
    for images, labels in anomaly_test_loader:
        images = images.to(DEVICE)
        outputs, _ = model_detection(images)
        
        # Calcul pixel par pixel de la MSE résiduelle
        images_flattened = images.view(images.size(0), -1)
        errors = ((outputs - images_flattened) ** 2).mean(dim=1)

        all_errors.extend(errors.cpu().numpy())
        all_labels.extend(labels.numpy())

all_errors = np.array(all_errors)
all_labels = np.array(all_labels)

# Étalonnage statistique du seuil au 95e percentile des données normales
erreurs_normales = all_errors[all_labels != CLASSE_ANOMALIE]
SEUIL = np.percentile(erreurs_normales, 95)
print(f"\nSeuil de décision fixé à : {SEUIL:.5f}")

# --- 4. Génération de l'histogramme de distribution ---
plt.figure(figsize=(10, 5))
plt.hist(all_errors[all_labels != CLASSE_ANOMALIE], bins=80, alpha=0.6, color='steelblue', label='Classes normales (0-8)')
plt.hist(all_errors[all_labels == CLASSE_ANOMALIE], bins=80, alpha=0.6, color='tomato', label=f'Anomalie ({CLASSE_ANOMALIE})')
plt.axvline(SEUIL, color='black', linewidth=2, linestyle='--', label=f'Seuil tau = {SEUIL:.4f}')
plt.xlabel('Erreur de reconstruction (MSE)')
plt.ylabel("Nombre d'images")
plt.title('Séparation des anomalies par seuillage résiduel')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('distribution_erreurs.png', dpi=150)
plt.show()

# --- 5. Génération des diagnostics visuels ---
def save_diagnostic_plots(title, indices, filename, n=6):
    fig, axes = plt.subplots(2, n, figsize=(2.5 * n, 5))
    fig.suptitle(title, fontsize=12, fontweight='bold')
    for col, idx in enumerate(indices[:n]):
        img, label = test_loader.dataset[idx]
        with torch.no_grad():
            recon, _ = model_detection(img.unsqueeze(0).to(DEVICE))
            recon = recon.view(28, 28).cpu().numpy()
        
        axes[0, col].imshow(img.squeeze().numpy(), cmap='gray')
        axes[0, col].axis('off')
        axes[1, col].imshow(recon, cmap='gray')
        axes[1, col].set_title(f'MSE: {all_errors[idx]:.4f}', fontsize=9)
        axes[1, col].axis('off')
    plt.tight_layout()
    plt.savefig(filename, dpi=150)
    plt.show()

idx_normaux = np.where((all_labels != CLASSE_ANOMALIE) & (all_errors <= SEUIL))[0]
idx_anomalies = np.where((all_labels == CLASSE_ANOMALIE) & (all_errors > SEUIL))[0]

save_diagnostic_plots('Classes normales - bien reconstruites', idx_normaux, 'classes_normales_bien_reconstruites.png')
save_diagnostic_plots('Anomalies (9) - detectees', idx_anomalies, 'anomalies_9_detectees.png')

# --- 6. Graphes de synthèse ---

# 6.a Comparaison en barres
modeles = ['Vanilla AE', 'Sparse AE', 'Denoising AE', 'VAE', 'AE (Anomalie)']

mse_finales = [
    0.0050,  
    0.0080,  
    0.0120,  
    0.0150,  
    history_anom['train_loss'][-1]
]

plt.figure(figsize=(10, 5))
bars = plt.bar(modeles, mse_finales, color=['steelblue', 'darkorange', 'seagreen', 'crimson', 'purple'], alpha=0.8)

plt.ylabel('Erreur finale de reconstruction (MSE)')
plt.title("Comparaison des performances des différentes architectures")
plt.grid(axis='y', alpha=0.3)

for bar in bars:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2, yval + 0.0002, f"{yval:.4f}", ha='center', va='bottom', fontsize=10)

plt.tight_layout()
plt.savefig('comparaison_modeles.png', dpi=150)
plt.show()

# 6.b Courbe de convergence
plt.figure(figsize=(10, 4))
plt.plot(range(1, 21), history_anom['train_loss'], label='Entraînement (Train Loss)', color='steelblue', linewidth=2)
if 'val_loss' in history_anom:
    plt.plot(range(1, 21), history_anom['val_loss'], label='Validation (Val Loss)', color='darkorange', linestyle='--', linewidth=2)

plt.xlabel('Époques')
plt.ylabel('Erreur (MSE)')
plt.title("Courbe d'apprentissage AE")
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('convergence_anomalie.png', dpi=150)
plt.show()