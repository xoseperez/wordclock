//-----------------------------------
// CONFIGURATION
//-----------------------------------

//grid_size=16;
//grid_step=10;
//grid_shift=0;

grid_size=8;
grid_step=8;
grid_shift=0.4;

grid_wall=1;
grid_height=5;
hole_size=4;
chamfer_size=4;

button_text = [
    "MODE",
    "LANGUAGE",
    "COLOR",
    "BRIGHTNESS"
];

list_esp_cat_16 = [
    "ésónesonvoracasi",
    //"undostreslangmig",
    "unodostreslngmig", //<- add "uno"
    "quarts1imenysmig",
    "cinc2ben34tocats",
    "lesdelasduesiset",
    "cincochovuit&una",
    "dosnuevediezonce",
    "quatreseiscuatro",
    "sietedoce5ymenos",
    "&onzedotzenoudeu",
    "veinticincomedia",
    "cuartodiezveinte",
    "ben6tocadespunto",
    "tocadapasadas789",
    "dellamatinitarda",
    "mañanatardenoche"
];

list_cat_8 = [
    "1dos3les",
    "laquarts",
    "de0dotze",
    "&unacinc",
    "duesiset",
    "quatres0",
    "vuitnou0",
    "deu&onze"
];

//-----------------------------------

module base(size, step, chamfer, hole) {
    size = (size+2)*step;
    difference() {
        translate([chamfer, chamfer, 0]) {
            minkowski() {
                square(size-2*chamfer);
                circle(chamfer, $fn=50);
            }
        }
        hole = hole / 2;
        hole_center = (step) / 2;
        translate([hole_center,hole_center,0]) {
            circle(hole, center=true, $fn=50);
        }
        translate([size-hole_center,hole_center,0]) {
            circle(hole, center=true, $fn=50);
        }
        translate([size-hole_center,size-hole_center,0]) {
            circle(hole, center=true, $fn=50);
        }
        translate([hole_center,size-hole_center,0]) {
            circle(hole, center=true, $fn=50);
        }
    }
}

module grid(size, step, wall) {
    for (x=[1:size]) {
        for (y=[1:size]) {
            translate([x*step+wall/2,y*step+wall/2,0]) {
                square(step-wall);
            }
        }
    }
}

module subgrid(size, step, wall) {
    for (x=[1:size]) {
        translate([x*step+wall/2,wall/2,wall]) {
            cube([step-wall,step-wall,step]);
        }
        translate([x*step+wall/2,(size+1)*step+wall/2,wall]) {
            cube([step-wall,step-wall,step]);
        }
    }
    for (y=[1:size]) {
        translate([wall/2,y*step+wall/2,wall]) {
            cube([step-wall,step-wall,step]);
        }
        translate([(size+1)*step+wall/2,y*step+wall/2,wall]) {
            cube([step-wall,step-wall,step]);
        }
    }
}

module mask()  {
        difference() {
            base(grid_size, grid_step, chamfer_size, hole_size);
            translate([0, grid_shift])
                grid(grid_size, grid_step, grid_wall);
        }
}

module 3dprint() {
    difference() {
        linear_extrude(grid_height) mask();
        subgrid(grid_size, grid_step, grid_wall);
    }
}

module arc(max, min) {
    union() {
        intersection() {
            difference() {
                circle(max, $fn=50);
                circle(min, $fn=50);
            }
            square(max,center=false);
        }
        translate([0,min]) square([5, 15-min], center=false);
    }
}

module layer1() {
    base(grid_size, grid_step, chamfer_size, hole_size);
}

module layer2() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([5,5,0]) arc(10,5);
        translate([(grid_size+2)*grid_step-5,5,0]) {
            mirror([1,0,0]) arc(10,5);
        }
    }
}

module layer2_v3() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([grid_step*1.5,grid_step*8 + 1,0]) {
                square([1.5*(grid_step-grid_wall),grid_wall*4], true);
        }
    }
}

module layer4() {
    border = grid_step/2;
    size = (grid_size+2)*grid_step - border*2;
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([border,border]) 
            difference() {
                square(size);
                circle(5, $fn=50);
                translate([size,0]) circle(5, $fn=50);
                translate([size,size]) circle(5, $fn=50);
                translate([0,size]) circle(5, $fn=50);
            }
    }
}

module wall_hole() {
    minkowski() {
        square([0.1,15], center=true);
        circle(3, $fn=50);
    }
}

module socket_hole() {
    circle(11.4/2, $fn=50);
}

module buttons() {
    //font = "Ubuntu:style=Bold";
    font = "AG Stencil:style=AG Stencil";
    for (x=[0:3]) {
        translate([0,x*12]) {
            circle(3.6, $fn=50);
            translate([6,-2.5]) 
                text(button_text[x], 6, font,"right","center");
        }
    }
}

module layer3() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([50,30,0]) wall_hole();
        translate([(grid_size+2)*grid_step-50,30,0]) wall_hole();
        translate([(180-51)/2-4,110+(180-80)/2+4,0])
            rotate(-90)
                pcbfootprint();
    }
}

module layer3_v2() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([30,30]) socket_hole();
        translate([30,50]) buttons();
        translate([100,110])
            rotate(-90)
                pcbfootprint();
    }
}

module pcbfootprint_v2() {
    
    hole_vector = sqrt(22*22*2);
    
    // PCB holes
    for(i = [45:90:315]) {
        rotate([0,0,i]) {
            translate([0,hole_vector]) {
                circle(1.5, $fn=50);
            }
        }
    }
    
    // speaker
    translate([1,3.5-22]) {
        circle(6.5, $fn=50);
    }
    

}

module layer3_v3() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([30,30]) socket_hole();
        translate([30,50]) circle(3.6, $fn=50);
        translate([90,90])
            pcbfootprint_v2();
    }
}

module layer3_v3b() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([15,15]) socket_hole();
        translate([15,30]) circle(3.6, $fn=50);
        translate([48,48])
            rotate([0,0,180]) pcbfootprint_v2();
    }
}

module layer5() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([grid_step,grid_step]) square([65,65]);
    }
}

module pcbfootprint(hole=4) {

    // HOLES
    hole = hole / 2;
    translate([4,4,0]) {
        circle(hole, center=true, $fn=50);
    }
    translate([84,4,0]) {
        circle(hole, center=true, $fn=50);
    }
    translate([4,55,0]) {
        circle(hole, center=true, $fn=50);
    }
    translate([84,55,0]) {
        circle(hole, center=true, $fn=50);
    }
    
    // BUTTONS
    //translate([23,44,0]) {
        //square([54,13]);
    //}
    
    // SPEAKER
    translate([49, 36]) circle(6.5, $fn=50);

}

module dowrite(char, x=2.7, y=2.5) {
    //font = "Ubuntu:style=Bold";
    font = "AG Stencil:style=AG Stencil";
    translate([x,y])
        text(char,6,font,"left","center");
}
module write(char) {
    char = (char == "&") ? "d'" : char;
    if (char == "m") {
        dowrite(char, 1.5);
    } else if (char == "w") {
        dowrite(char, 1.5);
    } else if (char == "i") {
        dowrite(char, 4);
    } else if (char == "í") {
        dowrite(char, 4);
    } else if (char == "l") {
        dowrite(char, 4);
    } else if (char == "g") {
        dowrite(char, y=3);
    } else if (char == "q") {
        dowrite(char, y=3);
    } else if (char == "y") {
        dowrite(char, y=3);
    } else if (char == "t") {
        dowrite(char, 3.5);
    } else if (char == "s") {
        dowrite(char, 3.5);
    } else {
        dowrite(char, 2.7);
    }
}

module chars(list) {
    
    for (y = [0 : 15]) {
        for (x = [ 0 : 15 ]) {
            translate([(x+1)*grid_step,grid_step*grid_size-y*grid_step]) write(list[y][x]);
        }
    }
    
    // Fix "matí"
    translate([94.8,28])
        offset(0.5)
            polygon([[0,0],[1,0.60],[1,0.61]]);
        
}

module vinil(list) {
    offset(0.10)
        difference() {
            layer1();
            chars(list);
        }
}

//Capa 1 (18x18, Acrílic 3mm, transparent, trasnslúcid i fosc)
//layer1(); 

//Graella (18x18, PLA negre)
//3dprint();
//mask();

//Capa 2 (18x18, Acrílic tranparent/fosc? 3mm)
//layer2();
//layer2_v3();

// Hollow layer
//layer4();

// PCB matrix layer
//layer5();

//Capa 3 (18x18, Acrílic transparent/fosc? 3mm)
//layer3();
//layer3_v2();
//layer3_v3();
layer3_v3b();

//Capes (transparent, 3mm)
//layer1();
//translate([184,0,0]) layer2();
//translate([2*184,0,0]) layer3();

// Lletres (18x18, vinil negre)
//vinil(list_cat_8);
