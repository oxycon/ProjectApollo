
bottle_radius = 100;
bottle_height = 300;
bottle_distance = 300;
bottle_total_height = bottle_distance + 2 * bottle_radius;

cap_radius = 20;
cap_height = 30;

t_slot_thickness = 15;
t_slot_h_clearance = 50;
t_slot_h_distance = bottle_distance + 2 * bottle_radius + t_slot_h_clearance;

t_slot_v_clearance = 150;
t_slot_v_length = bottle_total_height + 2 * t_slot_v_clearance;

plate_thickness = 5;

module bottle(height, radius, bottle_color, bottle_alpha) {
    color(bottle_color, alpha=bottle_alpha)
    hull() {
        sphere(r = radius, center = true);

        translate([0,0, height])
        sphere(r = radius, center = true);
    }
    
    total_height = height + 2 * radius;
    
    color("white", alpha=1)
    translate([0,0, - radius])
    cylinder(r = cap_radius, h = cap_height, center = true);

    color("white", alpha=1)
    translate([0,0, total_height - radius])
    cylinder(r = cap_radius, h = cap_height, center = true);
}

module bottles(bottle_height, bottle_radius, bottle_distance)
{
    translate([bottle_distance/2, bottle_distance/2, 0])
    bottle(bottle_height, bottle_radius, "LightSkyBlue", 0.25);

    translate([-bottle_distance/2, bottle_distance/2, 0])
    bottle(bottle_height, bottle_radius, "Yellow", 1);

    translate([bottle_distance/2, -bottle_distance/2, 0])
    bottle(bottle_height, bottle_radius, "Yellow", 1);

    translate([-bottle_distance/2, -bottle_distance/2, 0])
    bottle(bottle_height, bottle_radius, "LightSkyBlue", 0.25);
}

module t_slot(t_slot_thickness, t_slot_length)
{
    color("gray")
    cube([t_slot_thickness,  t_slot_thickness, t_slot_length], center=false);   
}


module v_t_slots(t_slot_h_distance, t_slot_thickness, t_slot_length)
{
    translate([t_slot_h_distance/2, t_slot_h_distance/2, 0])
    t_slot(t_slot_thickness, t_slot_length);

    translate([t_slot_h_distance/2, -t_slot_h_distance/2, 0])
    t_slot(t_slot_thickness, t_slot_length);

    translate([-t_slot_h_distance/2, t_slot_h_distance/2, 0])
    t_slot(t_slot_thickness, t_slot_length);  
    
    translate([-t_slot_h_distance/2, -t_slot_h_distance/2, 0])
    t_slot(t_slot_thickness, t_slot_length);
    

    translate([-t_slot_h_distance/2, -t_slot_h_distance/2, t_slot_v_clearance])
    rotate([0, 90, 0])
    t_slot(t_slot_thickness, t_slot_h_distance);

    translate([-t_slot_h_distance/2, t_slot_h_distance/2, t_slot_v_clearance])
    rotate([0, 90, 0])
    t_slot(t_slot_thickness, t_slot_h_distance);

    translate([-t_slot_h_distance/2, t_slot_h_distance/2, t_slot_v_clearance - t_slot_thickness])
    rotate([90, 0, 0])
    t_slot(t_slot_thickness, t_slot_h_distance);

    translate([t_slot_h_distance/2, t_slot_h_distance/2, t_slot_v_clearance - t_slot_thickness])
    rotate([90, 0, 0])
    t_slot(t_slot_thickness, t_slot_h_distance);


    translate([-t_slot_h_distance/2, -t_slot_h_distance/2, t_slot_v_clearance + bottle_total_height])
    rotate([0, 90, 0])
    t_slot(t_slot_thickness, t_slot_h_distance);

    translate([-t_slot_h_distance/2, t_slot_h_distance/2, t_slot_v_clearance + bottle_total_height])
    rotate([0, 90, 0])
    t_slot(t_slot_thickness, t_slot_h_distance);

    translate([-t_slot_h_distance/2, t_slot_h_distance/2, t_slot_v_clearance - t_slot_thickness + bottle_total_height])
    rotate([90, 0, 0])
    t_slot(t_slot_thickness, t_slot_h_distance);

    translate([t_slot_h_distance/2, t_slot_h_distance/2, t_slot_v_clearance - t_slot_thickness + bottle_total_height])
    rotate([90, 0, 0])
    t_slot(t_slot_thickness, t_slot_h_distance);

}    

module plate(side, thickness, height)
{
    color("white", alpha=0.5)
    translate([0, 0, height])
    cube([side, side, thickness], center=true);
}





translate([0, 0, bottle_radius + t_slot_v_clearance])
bottles(bottle_height, bottle_radius, bottle_distance);

v_t_slots(t_slot_h_distance, t_slot_thickness, t_slot_v_length);

plate(t_slot_h_distance, plate_thickness, t_slot_v_clearance);
plate(t_slot_h_distance, plate_thickness, t_slot_v_clearance + bottle_total_height);
