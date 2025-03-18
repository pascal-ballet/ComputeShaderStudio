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
        { 0.01, 80,  15, 0xFF0000FF }, // Mercure
        { 0.008, 120,  30, 0xFF5500FF }, // Venus
        { 0.005, 150,  30, 0xFF55FF00 }, // Terre
        { 0.003, 200,  20, 0xFF0055FF }, // Mars
        { 0.0009, 250, 55, 0xFF00DDFF }, // Jupiter
        { 0.0005, 300, 40, 0xFF00EEFF }, // Saturne
        { 0.0003, 450, 50, 0xFFF5F5F5 }, // Uranus
        { 0.0002, 550, 45, 0xFFFF1100 }  // Neptune
    };
    
    // Affichage et couleur
    if (distSoleil <= Soleil) {
        data_0[p] = 0xFF0022FF; //rouge
        if(distSoleil <= Soleil - 200){
            data_0[p] = 0xFF00FFFF; //rouge
        }
        if(distSoleil <= Soleil - 1500){
            data_0[p] = 0xFFFFFFFF; //rouge
        }
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