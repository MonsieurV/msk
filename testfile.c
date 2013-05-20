int main() {
    file_init();
    // Etat initial : 3 5 1 0 2.
    ajoute(3);
    ajoute(5);
    ajoute(1);ajoute(0);ajoute(2);
    affic_queue();
    affic_file();
    printf("suivant()\n");
    suivant();
    affic_queue();
    affic_file();
    printf("retire()\n");
    retire(0);
    affic_queue();
    affic_file();
    printf("ajoute()\n");
    ajoute(6);
    affic_queue();
    affic_file();
}