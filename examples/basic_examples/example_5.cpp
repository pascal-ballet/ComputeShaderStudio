//Notes de jamais mettre d'accent
//--------------------------------------------------
//Ajout possibles : 
//------Etoiles random en blancs
//------Apparition des planetes non lineaires ou random
//------Esthetiques des planetes
//--------------------------------------------------

void main() {
    
    //Coordonnees pour la Suite -----------------------------------------------------------------------------------

    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    int cx = int(WSX) / 2;  
    int cy = int(WSY) / 2 - 60;  //centrage du systeme

    int dx = int(x) - cx;
    int dy = int(y) - cy;
    //-----------------------------------------------------------------------------------------------------------

    //Valeur pour Soleil -----------------------------------------------------------------------------------------

    int distSoleil = dx * dx + dy * dy;
    int Soleil = 50 * 50;

    //Tableau des planetes --------------------------------------------------------------------------------------

    struct Planete {
        float vitesse; // par rapport a step (vitesse * step)
        int distance; //par rapport au centre
        int rayon; // taille du cercle
        int couleur;
    };

    Planete planetes[8] = {
        { 0.01, 80,  15, 0xFFFFFF00 }, // Mercure
        { 0.008, 120,  30, 0xFFFFA500 }, // Venus
        { 0.005, 150,  30, 0xFF0000FF }, // Terre
        { 0.003, 190,  20, 0xFFFF0000 }, // Mars
        { 0.0009, 250, 60, 0xFF964B00 }, // Jupiter
        { 0.0005, 300, 55, 0xFFD2B48C }, // Saturne
        { 0.0003, 380, 50, 0xFF00FFFF }, // Uranus
        { 0.0002, 450, 45, 0xFF000080 }  // Neptune
    };
    
    // Affichage et couleur
    if (distSoleil <= Soleil) {
        data_0[p] = 0xFF0000FF; //rouge
    } else {
        for (int i = 0; i < 8; i++) { //pour chaque planete de planetes

            //vitesse de la planete ---------------------------------------------------
            float angle = step * planetes[i].vitesse; 
            float cosAngle = cos(angle);
            float sinAngle = sin(angle);
            //-------------------------------------------------------------------------

            //position de la planete par rapport au soleil ----------------------------
            int planetX = int(cx + planetes[i].distance * cosAngle);
            int planetY = int(cy + planetes[i].distance * sinAngle);
            //-------------------------------------------------------------------------

            int distPlanete = (int(x) - planetX) * (int(x) - planetX) + (int(y) - planetY) * (int(y) - planetY);
            int distTrait = dx * dx + dy * dy;

            //Conditions pour colorer le bonne endroit de la planete--------------------
            if (distPlanete <= planetes[i].rayon * planetes[i].rayon) {
                data_0[p] = planetes[i].couleur; //coloration de la planete
                return;
            } else if (distTrait <= (planetes[i].distance * planetes[i].distance) + 1000 && distTrait >= (planetes[i].distance * planetes[i].distance) - 1000) {
                data_0[p] = 0xFF808080; //coloration du trait de l'orbite
                return;
            }
            //-------------------------------------------------------------------------

        }
        data_0[p] = 0xFF000000; //sinon coloris en noir
    }
}