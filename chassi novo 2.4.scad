top_x = 20; //	largura fundo motor
top_y = 70; //	base fundo motor
bottom_x = 35; //largura superior motor
bottom_y = 101; //base superior motor 
height = 25; //altura suporte do motor
espessura =5;// espessura de toda base
h= 30; // altura do trapézio trás
b1=160; // base maior do trapézio trás (16cm) perry
b2=155;// base menor do trapézio trás (16cm) perry
r = 17.75; // raio da borda
b3=160;//base maior do trapézio frente(16cm)
b4=155;// base menor do trapézio frente(16cm)
h1=10; // altura trapezio frente
lm = b2-50; //largura do meio
module trap_cube() // Formato dos suportes dos motores
{
	difference(){
		hull(){
			translate([0,0,height])
				cube([top_x , top_y + 6, espessura], center=true);
            translate ([espessura,0,0])
				cube([bottom_x -12, bottom_y  , espessura], center=true);
			}

		hull(){
			translate([0,0,height - 3])
				cube([top_x , top_y +3 , espessura], center=true);
				cube([bottom_x - espessura  , bottom_y - espessura , espessura], center=true);
		}
	}
}
module triGuide (l,w,x) {
  translate([0,0,w])
    rotate(90,[0,1,0])
      linear_extrude (x){
        polygon([[0,0],[w,0],[w/2, sqrt(3*w*w/4)]]);
      }
}

module triGuideFit (l,w,x) {
  translate([0,0,w])
    rotate(90,[0,1,0])
      difference() {
        cube ([x,w,l]);
        translate ([0,-0.01,-0.01])
          triGuide(l+0.02,w);
      }
}
module suporte_motor_direito ()
{
    difference ()
    {
        trap_cube();
        translate ([8,20,height/2])
        rotate ([0,90,0])
        cylinder (h=8,r=4,$fn = 100);
    }
}
module suporte_motor_esquerdo ()
{
difference ()
    {
        trap_cube();
        translate ([8,-20,height/2])
        rotate ([0,90,0])
        cylinder (h=8,r=4,$fn = 100);
    }
}
module trapezio_traseiro ()
{
    linear_extrude(espessura, center = true, convexity = 10)
    polygon(points=[[b1-b2,0],[0,h],[b1,h],[b2,0]], paths=[[0,1,2,3,0]]);
}



module borda () // Extremidades arredondadas
{
  difference () 
    {
    scale ([4.5,1,1])cylinder (espessura, r =r, $fn = 100);
    translate([-b1/2,0,0])
    cube ([b1,r,espessura]);
    }   
}
module trapezio_borda_traseira ()
{
  union ()
  {
    borda();
    translate ([-b1/2,h,espessura/2])
    rotate ([180,0,0])
    trapezio_traseiro();
  }
}

module trapezio_frente ()
{
    linear_extrude(espessura, center = true, convexity = 10)
    polygon(points=[[(b3-b4),0],[0,h1],[b3,h1],[b4,0]], paths=[[0,1,2,3,0]]);
}
module trapezio_borda_frente()
{
  union ()
   {    
     borda();
     translate ([-80,h1,espessura/2])
     rotate ([180,0,0])
     trapezio_frente(); 
   }
}
module furos() //Espaçadores
{
    translate ([-70,10, 0])
    cylinder (h=8,r=2.25,$fn = 100);
    translate ([70, 10,0])
    cylinder (h=8,r=2.25,$fn = 100);
    translate ([-70, 135,0])
    cylinder (h=8,r=2.25,$fn = 100);
    translate ([70, 135,0])
    cylinder (h=8,r=2.25,$fn = 100);
}

module suporte_motores () // Suporte esquerdo e direito posicionados corretamente
{
    union ()
    {
       translate ([-(lm/2+15),top_y+(h1-espessura/2),espessura/2])
       rotate([180,0,0])
       suporte_motor_direito();
       translate ([lm/2 + 15,top_y+(h1-espessura/2),espessura/2])
       rotate([180,0,180])
       suporte_motor_esquerdo ();
    }    
}
module meio () //Retângulo central
{
   cube([lm,100,espessura]);
} 


module trilhas_energia ()//Trilhas p/ placa de energia
{
    rotate([90,90,0])
    translate([-8,49.5,-15])
    triGuideFit(3,3,67);
    rotate([90,-90,0])
    translate([5,49.5,-15])
    triGuideFit(3,3,67);
}
module base () //Sem os suportes dos motores
{
    union ()
    {
       trapezio_borda_traseira();
       translate ([0,h+100+h1/2,0])
       rotate([0,0,180]) 
       trapezio_borda_frente();
       translate([-lm/2,h,0])
       meio(); 
       trilhas_energia();
    } 
}
module base_furos () //Sem os suportes dos motores
{
   difference()
    {
        base();
        furos();
    }
}
module plataforma () //Base + suportes
{
    union()
    {
        base_furos();
        suporte_motores();
        trilhas_energia();
    }
}

module holes () // Furos de encaixe horizontais
{
    difference ()
    {
        cube([20,105,2.5]);
        translate([5,10,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([5,95,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([5,46,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([5,59,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([15,95,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([15,10,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([15,46,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([15,59,0])
        cylinder (h=2.5,r=3.5,$fn=100);
    }
}
module pins () // Pinos de encaixe horizontais
{
    union ()
    {
        cube([20,105,2.5]);
        translate([5,10,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([5,46,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([5,59,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([5,95,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([15,95,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([15,10,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([15,46,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([15,59,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
    }
}
module traseira ()//Traseira + suporte de motores
{
    difference()
    {
        plataforma();
        translate([-82.5,80,0])
        cube([165,75.75,40]);
    }
}
module traseira_base () //Traseira s/ suporte de motores
{
    difference()
    {
        base_furos();
        translate([-82.5,80,0])
        cube([165,75.75,40]);
    }
}
module traseira_base_pins () //Traseira segunda base (s/ motores) + pino de encaixe
{
    union ()
    {
        traseira_base();
        rotate([0,0,90])
        translate([80,-52.5,2.5])
        pins();
    }
}
module traseira_plataforma_pins ()//Traseira + suporte motores + pinos de encaixe
{
    union ()
    {
        traseira();
        rotate([0,0,90])
        translate([80,-52.5,2.5])
        pins();
    }
}
module frente ()//Parte frontal
{
    difference ()
    {
        plataforma();
        traseira();
        translate([-55,80,0])
        cube([110,20,6]);
       
    }
}
module frente_holes ()//Parte frontal com furos p/ encaixe
{
    union()
    { 
        //translate([0,0,0])
        frente();
        rotate([0,0,90])
        translate([80,-52.5,0])
        holes();
    }
}
module  frente_esquerda ()//Parte frontal esquerda 
{
    difference()
    {
        frente_holes();
        translate([-80,80,0])
        cube ([80,80, espessura]);
        translate([0,100,0])
        cube ([10,50,6]);
    }
}    

module vertical_holes () //Furos verticais frontais
{
     difference ()
    {
        cube([20,50,2.5]);
        translate([5,10,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([5,40,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([15,10,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([15,40,0])
        cylinder (h=2.5,r=3.5,$fn=100);
    }
}

module frente_esquerda_holes () //Parte frontal esquerda com furos
{
    union ()
    {
        frente_esquerda();
        translate([-10,100,0])
        vertical_holes();
    }
}
module frente_direita () //Parte frontal direita
{
    difference()
    {
        frente_holes();
        translate([0,70,0])
        cube ([80,100, 6]);
        translate([-10,100,0])
        cube ([10,50,6]);
     }
} 
module vertical_pins () //Pinos verticais frontais
{
    union()
    {
        cube([20,50,2.5]);
        translate([5,10,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([5,40,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([15,10,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([15,40,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
    }
}
module frente_direita_pins ()//Parte frontal direita com pinos de encaixe
{
    union ()
    {
        frente_direita();
        translate([-10,100,2.5])
        vertical_pins();
    }
}
module traseira_vertical_holes () //Furos verticais traseiros
{
     difference ()
    {
        cube([20,80,2.5]);
        translate([5,10,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([5,70,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([15,10,0])
        cylinder (h=2.5,r=3.5,$fn=100);
        translate([15,70,0])
        cylinder (h=2.5,r=3.5,$fn=100);
    }
}
module traseira_esquerda () //Parte traseira esquerda
{
    difference ()
    {
        traseira_plataforma_pins();
        translate([-80,-20,-30])
        cube ([80,150, 50]);
        translate([0,0,0])
        cube ([10,80,6]);
    }
}
module traseira_esquerda_holes () //Parte traseira esquerda com furos de encaixe
{
    union ()
    {
        traseira_esquerda();
        translate([-10,0,0])
        traseira_vertical_holes();
    }
}
module traseira_vertical_pins () //Pinos verticais traseiros
{
    union()
    {
        cube([20,80,2.5]);
        translate([5,10,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([5,70,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([15,10,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
        translate([15,70,-2.5])
        cylinder (h=2.5,r=3,$fn=100);
    }
}
module traseira_direita () //Parte traseira direita
{
    difference ()
    {
        traseira_plataforma_pins();
        translate([0,-20,-30])
        cube ([80,150, 50]);
        translate([-10,0,0])
        cube ([10,80,6]);
    } 
}  
module traseira_direita_pins () //Parte traseira direita com pinos de encaixe
{
    union()
    {
        traseira_direita();
        translate([-10,0,2.5])
        traseira_vertical_pins();
    }
}
module encaixe_completo () //Encaixe das 4 partes do chassi
{
    union ()
    {
        traseira_esquerda_holes();
        traseira_direita_pins();
        frente_direita_pins();
        frente_esquerda_holes();
    }
}
//plataforma();//Chassi sem encaixes
//encaixe_completo();//Chassi com encaixes
//traseira_esquerda_holes();
//traseira_direita_pins();
//frente_direita_pins();
//frente_esquerda_holes();
//frente_esquerda();
//frente_direita();
//traseira_plataforma_pins ();
//base_furos();
        