//-----------------------------------
// CONFIGURATION
//-----------------------------------

// General settings
grid_size=16;
grid_step=16;
grid_wall=1;
grid_height=5;
hole_size=4;
chamfer_size=4;
font = "AG Stencil:style=AG Stencil";

// Text for the 4 buttons in version 1
button_text = [
    "MODE",
    "LANGUAGE",
    "COLOR",
    "BRIGHTNESS"
];

// Stencil of characters for 16x16 matrix (catalan & spanish)
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

// Stencil of characters for the 8x8 matrix (catalan)
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

// Front face (translucid or dark acrylic, also for the difuser)
//layer_front(); 

// Character stencial (pass stencil array as argument)
//layer_stencil(list_esp_cat_16);

// Grid (choose either a 3D model or a 2D model to laser cut)
//layer_grid_3D();
//layer_grid_2D();

// Matrix support layer, with cutouts for the cables
//layer_support();

// Hollow layer
//layer_hollow();

// Back layer (use v1 for DaClock PCB v1.X, use v2 for v2+)
//layer_back_v1();
//layer_back_v2();

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

module layer_grid_2D()  {
        difference() {
            base(grid_size, grid_step, chamfer_size, hole_size);
            grid(grid_size, grid_step, grid_wall);
        }
}

module layer_grid_3D() {
    difference() {
        linear_extrude(grid_height) layer_grid_2D();
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

module layer_front() {
    base(grid_size, grid_step, chamfer_size, hole_size);
}

module layer_support() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([5,5,0]) arc(10,5);
        translate([(grid_size+2)*grid_step-5,5,0]) {
            mirror([1,0,0]) arc(10,5);
        }
    }
}

module layer_hollow() {
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
    for (x=[0:3]) {
        translate([0,x*12]) {
            circle(3.6, $fn=50);
            translate([6,-2.5]) 
                text(button_text[x], 6, font,"right","center");
        }
    }
}

module layer_back_v1() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([50,30,0]) wall_hole();
        translate([(grid_size+2)*grid_step-50,30,0]) wall_hole();
        translate([(180-51)/2-4,110+(180-80)/2+4,0])
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

module layer_back_v2() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([30,30]) socket_hole();
        translate([30,50]) circle(3.6, $fn=50);
        translate([90,90])
            pcbfootprint_v2();
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

module layer_stencil(list) {
    offset(0.10)
        difference() {
            layer_front();
            chars(list);
        }
}

