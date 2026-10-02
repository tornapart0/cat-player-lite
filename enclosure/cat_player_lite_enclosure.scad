// Cat Player Lite — compact, original enclosure for the under-$30 build.
// Units: millimetres. Measure the purchased modules before the full print.
// Set part to "front", "back", "assembly", or "fit_check".

part = "assembly";
$fn = 64;

head_width = 116;
head_height = 84;
ear_height = 22;
wall = 2.2;
front_depth = 8;
back_depth = 16;
fit_gap = 0.35;

screen_opening = [23, 13];
screen_center = [0, 8];
previous_center = [-35, 3];
next_center = [35, 3];
encoder_center = [37, -20];
screw_points = [[-39, 22], [39, 22], [-36, -26], [36, -26]];

module rounded_rectangle_2d(size, radius) {
    offset(r = radius)
        square([size[0] - 2 * radius, size[1] - 2 * radius], center = true);
}

module cat_outline_2d() {
    union() {
        scale([head_width / 2, head_height / 2]) circle(r = 1);
        polygon(points = [
            [-50, 23], [-42, 42 + ear_height], [-19, 34], [-15, 22]
        ]);
        polygon(points = [
            [50, 23], [42, 42 + ear_height], [19, 34], [15, 22]
        ]);
    }
}

module front_openings(depth) {
    translate([screen_center[0], screen_center[1], -1])
        linear_extrude(depth + 2) rounded_rectangle_2d(screen_opening, 1.5);

    for (p = [previous_center, next_center])
        translate([p[0], p[1], -1]) cylinder(d = 10.5, h = depth + 2);

    translate([encoder_center[0], encoder_center[1], -1])
        cylinder(d = 7.4, h = depth + 2);

    for (x = [-10 : 5 : 10])
        for (y = [-34 : 5 : -29])
            translate([x, y, -1]) cylinder(d = 2.5, h = depth + 2);
}

module screw_clearance(depth) {
    for (p = screw_points)
        translate([p[0], p[1], -1]) cylinder(d = 2.4, h = depth + 2);
}

module front_shell() {
    difference() {
        union() {
            difference() {
                linear_extrude(front_depth) cat_outline_2d();
                translate([0, 0, wall])
                    linear_extrude(front_depth - wall + 0.1)
                        offset(delta = -wall) cat_outline_2d();
            }
            for (p = screw_points)
                translate([p[0], p[1], wall]) cylinder(d = 7.2, h = front_depth - wall);
        }
        front_openings(front_depth);
        screw_clearance(front_depth);
    }
}

module back_shell() {
    difference() {
        union() {
            difference() {
                linear_extrude(back_depth) cat_outline_2d();
                translate([0, 0, wall])
                    linear_extrude(back_depth - wall + 0.1)
                        offset(delta = -wall) cat_outline_2d();
            }
            for (p = screw_points)
                translate([p[0], p[1], wall]) cylinder(d = 8, h = back_depth - wall - 1);
            translate([0, 0, back_depth - 1.9])
                linear_extrude(1.7)
                    difference() {
                        offset(delta = -wall - fit_gap) cat_outline_2d();
                        offset(delta = -wall - fit_gap - 1.3) cat_outline_2d();
                    }
        }

        // USB-C on the left side.
        translate([-head_width / 2, -5, back_depth * 0.55])
            cube([12, 11, 5], center = true);

        // DFPlayer microSD access at the bottom.
        translate([0, -head_height / 2, back_depth * 0.48])
            cube([19, 12, 4], center = true);

        for (p = screw_points)
            translate([p[0], p[1], back_depth - 10]) cylinder(d = 1.7, h = 12);
    }
}

module fit_check_coupon() {
    difference() {
        translate([0, 0, 1.1]) cube([94, 45, 2.2], center = true);
        translate([0, 8, -1])
            linear_extrude(5) rounded_rectangle_2d(screen_opening, 1.5);
        translate([-34, 5, -1]) cylinder(d = 10.5, h = 5);
        translate([34, 5, -1]) cylinder(d = 10.5, h = 5);
        translate([34, -14, -1]) cylinder(d = 7.4, h = 5);
    }
}

if (part == "front") {
    front_shell();
} else if (part == "back") {
    back_shell();
} else if (part == "fit_check") {
    fit_check_coupon();
} else {
    color([0.94, 0.57, 0.67, 0.78]) front_shell();
    color([0.32, 0.52, 0.74, 0.72])
        translate([0, 0, front_depth + back_depth + 5])
            mirror([0, 0, 1]) back_shell();
}
