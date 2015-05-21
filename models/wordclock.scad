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

module 3dprint() {
    difference() {
        linear_extrude(grid_height) {
            difference() {
                base(grid_size, grid_step, chamfer_size, hole_size);
                grid(grid_size, grid_step, grid_wall);
            }
        }
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

module face1() {
    base(grid_size, grid_step, chamfer_size, hole_size);
}

module face2() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([5,5,0]) arc(10,5);
        translate([(grid_size+2)*grid_step-5,5,0]) {
            mirror([1,0,0]) arc(10,5);
        }
    }
}

//face1();
//translate([181,0]) 
    face2();
//3dprint();
