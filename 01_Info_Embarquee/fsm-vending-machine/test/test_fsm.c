#include "unity.h"
#include "fsm.h"

// 1. setUp() est exécutée AVANT chaque test. 
// Elle remet la machine à zéro pour que les tests ne se polluent pas entre eux.
void setUp(void) {
    FSM_Init();
}

// 2. tearDown() est exécutée APRÈS chaque test (on la laisse vide ici).
void tearDown(void) {
}

// ==========================================
// MES TESTS UNITAIRES
// ==========================================

void test_initial_state_should_be_idle(void) {
    // On vérifie que la machine démarre bien dans l'état IDLE
    TEST_ASSERT_EQUAL(STATE_IDLE, FSM_GetCurrentState());
}

void test_menu_button_should_change_state_to_selection(void) {
    // Action : On envoie l'événement "Bouton Menu"
    FSM_Run(EVENT_BUTTON_MENU);
    
    // Vérification : L'état DOIT être SELECTION
    TEST_ASSERT_EQUAL(STATE_SELECTION, FSM_GetCurrentState());
}

void test_timeout_in_selection_should_return_to_idle(void) {
    // Action 1 : On va dans le menu
    FSM_Run(EVENT_BUTTON_MENU);
    
    // Action 2 : On simule un timeout (l'utilisateur ne fait rien)
    FSM_Run(EVENT_TIMEOUT);
    
    // Vérification : La machine DOIT être revenue en attente
    TEST_ASSERT_EQUAL(STATE_IDLE, FSM_GetCurrentState());
}

void test_coin_insertion_should_increase_balance(void) {
    // 1. Préparation : On navigue jusqu'à l'état de paiement
    FSM_Run(EVENT_BUTTON_MENU);      // La machine passe en SELECTION
    FSM_Run(EVENT_PRODUCT_SELECTED); // La machine passe en PAYMENT

    // 2. Action : On insère une pièce
    FSM_Run(EVENT_COIN_INSERTED);

    // 3. Vérifications :
    // A. La caisse doit maintenant contenir 50 centimes
    TEST_ASSERT_EQUAL(50, FSM_GetBalance());
    
    // B. L'état doit toujours être PAYMENT (car 50 centimes, ce n'est pas assez pour le produit à 150)
    TEST_ASSERT_EQUAL(STATE_PAYMENT, FSM_GetCurrentState());
}

// ==========================================
// LE CHEF D'ORCHESTRE DES TESTS
// ==========================================
int main(void) {
    UNITY_BEGIN(); // Démarrage du framework
    
    // On liste ici tous les tests à exécuter
    RUN_TEST(test_initial_state_should_be_idle);
    RUN_TEST(test_menu_button_should_change_state_to_selection);
    RUN_TEST(test_timeout_in_selection_should_return_to_idle);
    RUN_TEST(test_coin_insertion_should_increase_balance);
    
    return UNITY_END(); // Fin et affichage des résultats
}