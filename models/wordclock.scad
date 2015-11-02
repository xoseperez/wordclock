//-----------------------------------
// CONFIGURATION
//-----------------------------------
grid_size=16;
grid_step=10;
grid_wall=1;
grid_height=5;
hole_size=4;
chamfer_size=4;
//-----------------------------------


module base(size, step, chamfer, hole) {
    size = (size+2)*step;
    difference() {
        translate([chamfer, chamfer, 0]) {
            minkowski() {
                square(size-2*chamfer);
                circle(chamfer);
            }
        }
        hole = hole / 2;
        hole_center = (step) / 2;
        translate([hole_center,hole_center,0]) {
            circle(hole, center=true);
        }
        translate([size-hole_center,hole_center,0]) {
            circle(hole, center=true);
        }
        translate([size-hole_center,size-hole_center,0]) {
            circle(hole, center=true);
        }
        translate([hole_center,size-hole_center,0]) {
            circle(hole, center=true);
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
                circle(max);
                circle(min);
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

module wall_hole() {
    minkowski() {
        square([0.1,15], center=true);
        circle(3);
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

module pcbfootprint(hole=4) {

    // HOLES
    hole = hole / 2;
    translate([4,4,0]) {
        circle(hole, center=true);
    }
    translate([84,4,0]) {
        circle(hole, center=true);
    }
    translate([4,55,0]) {
        circle(hole, center=true);
    }
    translate([84,55,0]) {
        circle(hole, center=true);
    }
    
    // BUTTONS
    translate([23,44,0]) {
        square([54,13]);
    }

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

module chars() {
    list = [
        "ésónesonvoracasi",
        "undostreslangmig",
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
    for (y = [0 : 15]) {
        for (x = [ 0 : 15 ]) {
            translate([(x+1)*10,160-y*10]) write(list[y][x]);
        }
    }
    
    // Fix "matí"
    translate([94.8,28])
        offset(0.5)
            polygon([[0,0],[1,0.60],[1,0.61]]);
        
}

module vinil() {
    offset(0.10)
        difference() {
            layer1();
            chars();
        }
}

//Capa 1 (18x18, Acrílic 3mm, transparent, trasnslúcid i fosc)
//layer1(); 

//Capa 2 (18x18, Acrílic tranparent/fosc? 3mm)
//layer2();

//Capa 3 (18x18, Acrílic transparent/fosc? 3mm)
//layer3();

//Capes (transparent, 3mm)
//layer1();
//translate([184,0,0]) layer2();
//translate([2*184,0,0]) layer3();

//Graella (18x18, PLA negre)
//3dprint();

// Lletres (18x18, vinil negre)
//vinil();
