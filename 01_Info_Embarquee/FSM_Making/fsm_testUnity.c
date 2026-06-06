#include "unity.h"
#include "fsmFV.h"

// Exécuté avant CHAQUE test pour garantir une machine propre
void setUp(void) {
    Init_FSM();
}
void tearDown(void) {}

// --- TESTS NOMINAUX (Tout se passe bien) ---

void test_Nominal_Flow_Until_Payment(void) {
    Process_Event(PRES_BTN);
    TEST_ASSERT_EQUAL(SELECTION, Get_Current_State());
    
    Process_Event(SELECT_ITEM);
    TEST_ASSERT_EQUAL(PAYMENT, Get_Current_State());
}

void test_Nominal_Flow_Full_Cycle(void) {
    // On simule un client parfait
    Process_Event(PRES_BTN);
    Process_Event(SELECT_ITEM);
    Process_Event(INSERT_COIN);
    TEST_ASSERT_EQUAL(DISTRIBUTION, Get_Current_State());
    
    Process_Event(DISPENSE_ITEM);
    TEST_ASSERT_EQUAL(IDLE, Get_Current_State()); // La machine doit revenir au repos
}

void test_Payment_Return_Change(void) {
    Process_Event(PRES_BTN);
    Process_Event(SELECT_ITEM);
    Process_Event(RETURN_CHANGE); // Le client annule
    TEST_ASSERT_EQUAL(IDLE, Get_Current_State());
}

// --- TESTS AUX LIMITES / ERREURS ---

void test_Ignore_Invalid_Event_When_Idle(void) {
    Process_Event(INSERT_COIN); // On insère une pièce alors que la machine dort
    TEST_ASSERT_EQUAL(IDLE, Get_Current_State()); // Elle doit l'ignorer et rester IDLE
}

void test_Timeout_During_Selection_Returns_To_Idle(void) {
    Process_Event(PRES_BTN); // On passe en SELECTION
    Process_Event(TIMEOUT_30S); // On simule l'expiration du temps
    TEST_ASSERT_EQUAL(IDLE, Get_Current_State());
}

void test_Timeout_During_Payment_Goes_To_Error(void) {
    Process_Event(PRES_BTN);
    Process_Event(SELECT_ITEM); // On passe en PAYMENT
    Process_Event(TIMEOUT_30S); // On simule l'expiration du temps
    TEST_ASSERT_EQUAL(ERROR, Get_Current_State());
}

void test_Invalid_Action_During_Distribution(void) {
    Process_Event(PRES_BTN);
    Process_Event(SELECT_ITEM);
    Process_Event(INSERT_COIN); // On passe en DISTRIBUTION
    Process_Event(INVALID_ACTION); // Quelqu'un tire sur le gobelet trop tôt
    TEST_ASSERT_EQUAL(ERROR, Get_Current_State());
}

// --- MOTEUR DE TESTS ---
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_Nominal_Flow_Until_Payment);
    RUN_TEST(test_Nominal_Flow_Full_Cycle);
    RUN_TEST(test_Payment_Return_Change);
    RUN_TEST(test_Ignore_Invalid_Event_When_Idle);
    RUN_TEST(test_Timeout_During_Selection_Returns_To_Idle);
    RUN_TEST(test_Timeout_During_Payment_Goes_To_Error);
    RUN_TEST(test_Invalid_Action_During_Distribution);
    
    return UNITY_END();
}