#define WP_NO_BFLOAT16

#define WP_TILE_BLOCK_DIM 1
#define WP_NO_CRT
#include "builtin.h"
#include "deterministic.h"

// avoid namespacing of float type for casting to float type, this is to avoid wp::float(x), which is not valid in C++
#define float(x) cast_float(x)
#define adj_float(x, adj_x, adj_ret) adj_cast_float(x, adj_x, adj_ret)

#define int(x) cast_int(x)
#define adj_int(x, adj_x, adj_ret) adj_cast_int(x, adj_x, adj_ret)

#define builtin_tid1d() wp::tid(task_index, dim)
#define builtin_tid2d(x, y) wp::tid(x, y, task_index, dim)
#define builtin_tid3d(x, y, z) wp::tid(x, y, z, task_index, dim)
#define builtin_tid4d(x, y, z, w) wp::tid(x, y, z, w, task_index, dim)

#define builtin_block_dim() wp::block_dim()

struct wp_args_ray_image_9194dcf6 {
    wp::uint64 mesh;
    wp::array_t<wp::vec_t<3, wp::float32>> colors;
    wp::array_t<wp::int32> robot_faces;
    wp::int32 width;
    wp::int32 height;
    wp::float32 fx;
    wp::float32 fy;
    wp::float32 cx;
    wp::float32 cy;
    wp::vec_t<3, wp::float32> origin;
    wp::mat_t<3, 3, wp::float32> R;
    wp::int32 shaded;
    wp::float32 near_plane;
    wp::array_t<wp::float32> depth;
    wp::array_t<wp::vec_t<3, wp::float32>> rgb;
    wp::array_t<wp::int32> robot_mask;
};


void ray_image_9194dcf6_cpu_kernel_forward(
    wp::launch_bounds_t<1> dim,
    size_t task_index,
    wp_args_ray_image_9194dcf6 *_wp_args)
{
    //---------
    // argument vars
    wp::uint64 var_mesh = _wp_args->mesh;
    wp::array_t<wp::vec_t<3, wp::float32>> var_colors = _wp_args->colors;
    wp::array_t<wp::int32> var_robot_faces = _wp_args->robot_faces;
    wp::int32 var_width = _wp_args->width;
    wp::int32 var_height = _wp_args->height;
    wp::float32 var_fx = _wp_args->fx;
    wp::float32 var_fy = _wp_args->fy;
    wp::float32 var_cx = _wp_args->cx;
    wp::float32 var_cy = _wp_args->cy;
    wp::vec_t<3, wp::float32> var_origin = _wp_args->origin;
    wp::mat_t<3, 3, wp::float32> var_R = _wp_args->R;
    wp::int32 var_shaded = _wp_args->shaded;
    wp::float32 var_near_plane = _wp_args->near_plane;
    wp::array_t<wp::float32> var_depth = _wp_args->depth;
    wp::array_t<wp::vec_t<3, wp::float32>> var_rgb = _wp_args->rgb;
    wp::array_t<wp::int32> var_robot_mask = _wp_args->robot_mask;
    //---------
    // primal vars
    wp::int32 var_0;
    wp::int32 var_1;
    wp::int32 var_2;
    wp::float32 var_3;
    wp::float32 var_4;
    wp::float32 var_5;
    wp::float32 var_6;
    wp::float32 var_7;
    wp::float32 var_8;
    wp::float32 var_9;
    const wp::float32 var_10 = -1.0;
    wp::vec_t<3, wp::float32> var_11;
    wp::float32 var_12;
    wp::vec_t<3, wp::float32> var_13;
    wp::vec_t<3, wp::float32> var_14;
    wp::float32 var_15;
    wp::vec_t<3, wp::float32> var_16;
    wp::vec_t<3, wp::float32> var_17;
    const wp::float32 var_18 = 4.0;
    wp::mesh_query_ray_t var_19;
    const wp::int32 var_20 = -1;
    bool* var_21;
    bool var_22;
    wp::float32* var_23;
    wp::float32 var_24;
    wp::float32 var_25;
    wp::float32 var_26;
    wp::int32* var_27;
    wp::vec_t<3, wp::float32>* var_28;
    wp::int32 var_29;
    wp::vec_t<3, wp::float32> var_30;
    wp::int32* var_31;
    wp::int32* var_32;
    wp::int32 var_33;
    wp::int32 var_34;
    const wp::int32 var_35 = 1;
    bool var_36;
    const wp::float32 var_37 = 0.35;
    const wp::float32 var_38 = 0.65;
    wp::vec_t<3, wp::float32>* var_39;
    const wp::float32 var_40 = 0.5;
    const wp::float32 var_41 = -0.6;
    const wp::float32 var_42 = 0.8;
    wp::vec_t<3, wp::float32> var_43;
    wp::vec_t<3, wp::float32> var_44;
    wp::float32 var_45;
    wp::vec_t<3, wp::float32> var_46;
    wp::float32 var_47;
    wp::float32 var_48;
    wp::float32 var_49;
    wp::vec_t<3, wp::float32>* var_50;
    wp::vec_t<3, wp::float32> var_51;
    wp::vec_t<3, wp::float32> var_52;
    bool var_53;
    bool var_54;
    const wp::float32 var_55 = 0.0;
    const wp::float32 var_56 = 0.025;
    const wp::float32 var_57 = 0.03;
    const wp::float32 var_58 = 0.04;
    wp::vec_t<3, wp::float32> var_59;
    bool var_60;
    //---------
    // forward
    // def ray_image(mesh:wp.uint64,colors:wp.array(dtype=wp.vec3),robot_faces:wp.array(dtype=wp.int32),width:int,height:int,fx:float,fy:float,cx:float,cy:float,origin:wp.vec3,R:wp.mat33,shaded:int,near_plane:float,depth:wp.array(dtype=float),rgb:wp.array(dtype=wp.vec3),robot_mask:wp.array(dtype=wp.int32)):       <L 5>
    // i=wp.tid();u=i%width;v=i//width                                                        <L 6>
    var_0 = builtin_tid1d();
    var_1 = wp::mod(var_0, var_width);
    var_2 = wp::floordiv(var_0, var_width);
    // local=wp.vec3((float(u)-cx)/fx,-(float(v)-cy)/fy,-1.)                                  <L 7>
    var_3 = wp::float(var_1);
    var_4 = wp::sub(var_3, var_cx);
    var_5 = wp::div(var_4, var_fx);
    var_6 = wp::float(var_2);
    var_7 = wp::sub(var_6, var_cy);
    var_8 = wp::neg(var_7);
    var_9 = wp::div(var_8, var_fy);
    var_11 = wp::vec_t<3, wp::float32>(var_5, var_9, var_10);
    // length=wp.length(local);direction=R*(local/length)                                     <L 8>
    var_12 = wp::length(var_11);
    var_13 = wp::div(var_11, var_12);
    var_14 = wp::mul(var_R, var_13);
    // near=near_plane*length                                                                 <L 11>
    var_15 = wp::mul(var_near_plane, var_12);
    // hit=wp.mesh_query_ray(mesh,origin+direction*near,direction,4.)                         <L 12>
    var_16 = wp::mul(var_14, var_15);
    var_17 = wp::add(var_origin, var_16);
    var_19 = wp::mesh_query_ray(var_mesh, var_17, var_14, var_18, var_20);
    // if hit.result:                                                                         <L 13>
    var_21 = &((var_19).result);
    var_22 = wp::load(var_21);
    if (var_22) {
        // depth[i]=(hit.t+near)/length                                                       <L 14>
        var_23 = &((var_19).t);
        var_25 = wp::load(var_23);
        var_24 = wp::add(var_25, var_15);
        var_26 = wp::div(var_24, var_12);
        wp::array_store(var_depth, var_0, var_26);
        // rgb[i]=colors[hit.face]                                                            <L 15>
        var_27 = &((var_19).face);
        var_29 = wp::load(var_27);
        var_28 = wp::address(var_colors, var_29);
        var_30 = wp::load(var_28);
        wp::array_store(var_rgb, var_0, var_30);
        // robot_mask[i]=robot_faces[hit.face]                                                <L 16>
        var_31 = &((var_19).face);
        var_33 = wp::load(var_31);
        var_32 = wp::address(var_robot_faces, var_33);
        var_34 = wp::load(var_32);
        wp::array_store(var_robot_mask, var_0, var_34);
        // if shaded==1:                                                                      <L 17>
        var_36 = (var_shaded == var_35);
        if (var_36) {
            // illumination=.35+.65*wp.abs(wp.dot(hit.normal,wp.normalize(wp.vec3(.5,-.6,.8))))       <L 18>
            var_39 = &((var_19).normal);
            var_43 = wp::vec_t<3, wp::float32>(var_40, var_41, var_42);
            var_44 = wp::normalize(var_43);
            var_46 = wp::load(var_39);
            var_45 = wp::dot(var_46, var_44);
            var_47 = wp::abs(var_45);
            var_48 = wp::mul(var_38, var_47);
            var_49 = wp::add(var_37, var_48);
            // rgb[i]=rgb[i]*illumination                                                     <L 19>
            var_50 = wp::address(var_rgb, var_0);
            var_52 = wp::load(var_50);
            var_51 = wp::mul(var_52, var_49);
            wp::array_store(var_rgb, var_0, var_51);
        }
    }
    var_53 = wp::load(var_21);
    var_54 = wp::load(var_21);
    if (!var_54) {
        // depth[i]=0.                                                                        <L 21>
        wp::array_store(var_depth, var_0, var_55);
        // rgb[i]=wp.vec3(.025,.03,.04)                                                       <L 22>
        var_59 = wp::vec_t<3, wp::float32>(var_56, var_57, var_58);
        wp::array_store(var_rgb, var_0, var_59);
    }
    var_60 = wp::load(var_21);
}



void ray_image_9194dcf6_cpu_kernel_backward(
    wp::launch_bounds_t<1> dim,
    size_t task_index,
    wp_args_ray_image_9194dcf6 *_wp_args,
    wp_args_ray_image_9194dcf6 *_wp_adj_args)
{
    //---------
    // argument vars
    wp::uint64 var_mesh = _wp_args->mesh;
    wp::array_t<wp::vec_t<3, wp::float32>> var_colors = _wp_args->colors;
    wp::array_t<wp::int32> var_robot_faces = _wp_args->robot_faces;
    wp::int32 var_width = _wp_args->width;
    wp::int32 var_height = _wp_args->height;
    wp::float32 var_fx = _wp_args->fx;
    wp::float32 var_fy = _wp_args->fy;
    wp::float32 var_cx = _wp_args->cx;
    wp::float32 var_cy = _wp_args->cy;
    wp::vec_t<3, wp::float32> var_origin = _wp_args->origin;
    wp::mat_t<3, 3, wp::float32> var_R = _wp_args->R;
    wp::int32 var_shaded = _wp_args->shaded;
    wp::float32 var_near_plane = _wp_args->near_plane;
    wp::array_t<wp::float32> var_depth = _wp_args->depth;
    wp::array_t<wp::vec_t<3, wp::float32>> var_rgb = _wp_args->rgb;
    wp::array_t<wp::int32> var_robot_mask = _wp_args->robot_mask;
    wp::uint64 adj_mesh = _wp_adj_args->mesh;
    wp::array_t<wp::vec_t<3, wp::float32>> adj_colors = _wp_adj_args->colors;
    wp::array_t<wp::int32> adj_robot_faces = _wp_adj_args->robot_faces;
    wp::int32 adj_width = _wp_adj_args->width;
    wp::int32 adj_height = _wp_adj_args->height;
    wp::float32 adj_fx = _wp_adj_args->fx;
    wp::float32 adj_fy = _wp_adj_args->fy;
    wp::float32 adj_cx = _wp_adj_args->cx;
    wp::float32 adj_cy = _wp_adj_args->cy;
    wp::vec_t<3, wp::float32> adj_origin = _wp_adj_args->origin;
    wp::mat_t<3, 3, wp::float32> adj_R = _wp_adj_args->R;
    wp::int32 adj_shaded = _wp_adj_args->shaded;
    wp::float32 adj_near_plane = _wp_adj_args->near_plane;
    wp::array_t<wp::float32> adj_depth = _wp_adj_args->depth;
    wp::array_t<wp::vec_t<3, wp::float32>> adj_rgb = _wp_adj_args->rgb;
    wp::array_t<wp::int32> adj_robot_mask = _wp_adj_args->robot_mask;
    //---------
    // primal vars
    wp::int32 var_0;
    wp::int32 var_1;
    wp::int32 var_2;
    wp::float32 var_3;
    wp::float32 var_4;
    wp::float32 var_5;
    wp::float32 var_6;
    wp::float32 var_7;
    wp::float32 var_8;
    wp::float32 var_9;
    const wp::float32 var_10 = -1.0;
    wp::vec_t<3, wp::float32> var_11;
    wp::float32 var_12;
    wp::vec_t<3, wp::float32> var_13;
    wp::vec_t<3, wp::float32> var_14;
    wp::float32 var_15;
    wp::vec_t<3, wp::float32> var_16;
    wp::vec_t<3, wp::float32> var_17;
    const wp::float32 var_18 = 4.0;
    wp::mesh_query_ray_t var_19;
    const wp::int32 var_20 = -1;
    bool* var_21;
    bool var_22;
    wp::float32* var_23;
    wp::float32 var_24;
    wp::float32 var_25;
    wp::float32 var_26;
    wp::int32* var_27;
    wp::vec_t<3, wp::float32>* var_28;
    wp::int32 var_29;
    wp::vec_t<3, wp::float32> var_30;
    wp::int32* var_31;
    wp::int32* var_32;
    wp::int32 var_33;
    wp::int32 var_34;
    const wp::int32 var_35 = 1;
    bool var_36;
    const wp::float32 var_37 = 0.35;
    const wp::float32 var_38 = 0.65;
    wp::vec_t<3, wp::float32>* var_39;
    const wp::float32 var_40 = 0.5;
    const wp::float32 var_41 = -0.6;
    const wp::float32 var_42 = 0.8;
    wp::vec_t<3, wp::float32> var_43;
    wp::vec_t<3, wp::float32> var_44;
    wp::float32 var_45;
    wp::vec_t<3, wp::float32> var_46;
    wp::float32 var_47;
    wp::float32 var_48;
    wp::float32 var_49;
    wp::vec_t<3, wp::float32>* var_50;
    wp::vec_t<3, wp::float32> var_51;
    wp::vec_t<3, wp::float32> var_52;
    bool var_53;
    bool var_54;
    const wp::float32 var_55 = 0.0;
    const wp::float32 var_56 = 0.025;
    const wp::float32 var_57 = 0.03;
    const wp::float32 var_58 = 0.04;
    wp::vec_t<3, wp::float32> var_59;
    bool var_60;
    //---------
    // dual vars
    wp::int32 adj_0 = {};
    wp::int32 adj_1 = {};
    wp::int32 adj_2 = {};
    wp::float32 adj_3 = {};
    wp::float32 adj_4 = {};
    wp::float32 adj_5 = {};
    wp::float32 adj_6 = {};
    wp::float32 adj_7 = {};
    wp::float32 adj_8 = {};
    wp::float32 adj_9 = {};
    wp::float32 adj_10 = {};
    wp::vec_t<3, wp::float32> adj_11 = {};
    wp::float32 adj_12 = {};
    wp::vec_t<3, wp::float32> adj_13 = {};
    wp::vec_t<3, wp::float32> adj_14 = {};
    wp::float32 adj_15 = {};
    wp::vec_t<3, wp::float32> adj_16 = {};
    wp::vec_t<3, wp::float32> adj_17 = {};
    wp::float32 adj_18 = {};
    wp::mesh_query_ray_t adj_19 = {};
    wp::int32 adj_20 = {};
    bool adj_21 = {};
    bool adj_22 = {};
    wp::float32 adj_23 = {};
    wp::float32 adj_24 = {};
    wp::float32 adj_25 = {};
    wp::float32 adj_26 = {};
    wp::int32 adj_27 = {};
    wp::vec_t<3, wp::float32> adj_28 = {};
    wp::int32 adj_29 = {};
    wp::vec_t<3, wp::float32> adj_30 = {};
    wp::int32 adj_31 = {};
    wp::int32 adj_32 = {};
    wp::int32 adj_33 = {};
    wp::int32 adj_34 = {};
    wp::int32 adj_35 = {};
    bool adj_36 = {};
    wp::float32 adj_37 = {};
    wp::float32 adj_38 = {};
    wp::vec_t<3, wp::float32> adj_39 = {};
    wp::float32 adj_40 = {};
    wp::float32 adj_41 = {};
    wp::float32 adj_42 = {};
    wp::vec_t<3, wp::float32> adj_43 = {};
    wp::vec_t<3, wp::float32> adj_44 = {};
    wp::float32 adj_45 = {};
    wp::vec_t<3, wp::float32> adj_46 = {};
    wp::float32 adj_47 = {};
    wp::float32 adj_48 = {};
    wp::float32 adj_49 = {};
    wp::vec_t<3, wp::float32> adj_50 = {};
    wp::vec_t<3, wp::float32> adj_51 = {};
    wp::vec_t<3, wp::float32> adj_52 = {};
    bool adj_53 = {};
    bool adj_54 = {};
    wp::float32 adj_55 = {};
    wp::float32 adj_56 = {};
    wp::float32 adj_57 = {};
    wp::float32 adj_58 = {};
    wp::vec_t<3, wp::float32> adj_59 = {};
    bool adj_60 = {};
    //---------
    // forward
    // def ray_image(mesh:wp.uint64,colors:wp.array(dtype=wp.vec3),robot_faces:wp.array(dtype=wp.int32),width:int,height:int,fx:float,fy:float,cx:float,cy:float,origin:wp.vec3,R:wp.mat33,shaded:int,near_plane:float,depth:wp.array(dtype=float),rgb:wp.array(dtype=wp.vec3),robot_mask:wp.array(dtype=wp.int32)):       <L 5>
    // i=wp.tid();u=i%width;v=i//width                                                        <L 6>
    var_0 = builtin_tid1d();
    var_1 = wp::mod(var_0, var_width);
    var_2 = wp::floordiv(var_0, var_width);
    // local=wp.vec3((float(u)-cx)/fx,-(float(v)-cy)/fy,-1.)                                  <L 7>
    var_3 = wp::float(var_1);
    var_4 = wp::sub(var_3, var_cx);
    var_5 = wp::div(var_4, var_fx);
    var_6 = wp::float(var_2);
    var_7 = wp::sub(var_6, var_cy);
    var_8 = wp::neg(var_7);
    var_9 = wp::div(var_8, var_fy);
    var_11 = wp::vec_t<3, wp::float32>(var_5, var_9, var_10);
    // length=wp.length(local);direction=R*(local/length)                                     <L 8>
    var_12 = wp::length(var_11);
    var_13 = wp::div(var_11, var_12);
    var_14 = wp::mul(var_R, var_13);
    // near=near_plane*length                                                                 <L 11>
    var_15 = wp::mul(var_near_plane, var_12);
    // hit=wp.mesh_query_ray(mesh,origin+direction*near,direction,4.)                         <L 12>
    var_16 = wp::mul(var_14, var_15);
    var_17 = wp::add(var_origin, var_16);
    var_19 = wp::mesh_query_ray(var_mesh, var_17, var_14, var_18, var_20);
    // if hit.result:                                                                         <L 13>
    var_21 = &((var_19).result);
    var_22 = wp::load(var_21);
    if (var_22) {
        // depth[i]=(hit.t+near)/length                                                       <L 14>
        var_23 = &((var_19).t);
        var_25 = wp::load(var_23);
        var_24 = wp::add(var_25, var_15);
        var_26 = wp::div(var_24, var_12);
        // wp::array_store(var_depth, var_0, var_26);
        // rgb[i]=colors[hit.face]                                                            <L 15>
        var_27 = &((var_19).face);
        var_29 = wp::load(var_27);
        var_28 = wp::address(var_colors, var_29);
        var_30 = wp::load(var_28);
        // wp::array_store(var_rgb, var_0, var_30);
        // robot_mask[i]=robot_faces[hit.face]                                                <L 16>
        var_31 = &((var_19).face);
        var_33 = wp::load(var_31);
        var_32 = wp::address(var_robot_faces, var_33);
        var_34 = wp::load(var_32);
        // wp::array_store(var_robot_mask, var_0, var_34);
        // if shaded==1:                                                                      <L 17>
        var_36 = (var_shaded == var_35);
        if (var_36) {
            // illumination=.35+.65*wp.abs(wp.dot(hit.normal,wp.normalize(wp.vec3(.5,-.6,.8))))       <L 18>
            var_39 = &((var_19).normal);
            var_43 = wp::vec_t<3, wp::float32>(var_40, var_41, var_42);
            var_44 = wp::normalize(var_43);
            var_46 = wp::load(var_39);
            var_45 = wp::dot(var_46, var_44);
            var_47 = wp::abs(var_45);
            var_48 = wp::mul(var_38, var_47);
            var_49 = wp::add(var_37, var_48);
            // rgb[i]=rgb[i]*illumination                                                     <L 19>
            var_50 = wp::address(var_rgb, var_0);
            var_52 = wp::load(var_50);
            var_51 = wp::mul(var_52, var_49);
            // wp::array_store(var_rgb, var_0, var_51);
        }
    }
    var_53 = wp::load(var_21);
    var_54 = wp::load(var_21);
    if (!var_54) {
        // depth[i]=0.                                                                        <L 21>
        // wp::array_store(var_depth, var_0, var_55);
        // rgb[i]=wp.vec3(.025,.03,.04)                                                       <L 22>
        var_59 = wp::vec_t<3, wp::float32>(var_56, var_57, var_58);
        // wp::array_store(var_rgb, var_0, var_59);
    }
    var_60 = wp::load(var_21);
    //---------
    // reverse
    if (!var_60) {
        wp::adj_array_store(var_rgb, var_0, var_59, adj_rgb, adj_0, adj_59);
        wp::adj_vec_t(var_56, var_57, var_58, adj_56, adj_57, adj_58, adj_59);
        // adj: rgb[i]=wp.vec3(.025,.03,.04)                                                  <L 22>
        wp::adj_array_store(var_depth, var_0, var_55, adj_depth, adj_0, adj_55);
        // adj: depth[i]=0.                                                                   <L 21>
    }
    if (var_53) {
        if (var_36) {
            wp::adj_array_store(var_rgb, var_0, var_51, adj_rgb, adj_0, adj_51);
            wp::adj_mul(var_52, var_49, adj_50, adj_49, adj_51);
            wp::adj_address(var_rgb, var_0, adj_rgb, adj_0, adj_50);
            // adj: rgb[i]=rgb[i]*illumination                                                <L 19>
            wp::adj_add(var_37, var_48, adj_37, adj_48, adj_49);
            wp::adj_mul(var_38, var_47, adj_38, adj_47, adj_48);
            wp::adj_abs(var_45, adj_45, adj_47);
            wp::adj_dot(var_46, var_44, adj_39, adj_44, adj_45);
            wp::adj_normalize(var_43, var_44, adj_43, adj_44);
            wp::adj_vec_t(var_40, var_41, var_42, adj_40, adj_41, adj_42, adj_43);
            adj_19.normal += adj_39;
            // adj: illumination=.35+.65*wp.abs(wp.dot(hit.normal,wp.normalize(wp.vec3(.5,-.6,.8))))  <L 18>
        }
        // adj: if shaded==1:                                                                 <L 17>
        wp::adj_array_store(var_robot_mask, var_0, var_34, adj_robot_mask, adj_0, adj_32);
        wp::adj_address(var_robot_faces, var_33, adj_robot_faces, adj_31, adj_32);
        adj_19.face = adj_31;
        // adj: robot_mask[i]=robot_faces[hit.face]                                           <L 16>
        wp::adj_array_store(var_rgb, var_0, var_30, adj_rgb, adj_0, adj_28);
        wp::adj_address(var_colors, var_29, adj_colors, adj_27, adj_28);
        adj_19.face = adj_27;
        // adj: rgb[i]=colors[hit.face]                                                       <L 15>
        wp::adj_array_store(var_depth, var_0, var_26, adj_depth, adj_0, adj_26);
        wp::adj_div(var_24, var_12, var_26, adj_24, adj_12, adj_26);
        wp::adj_add(var_25, var_15, adj_23, adj_15, adj_24);
        adj_19.t += adj_23;
        // adj: depth[i]=(hit.t+near)/length                                                  <L 14>
    }
    adj_19.result = adj_21;
    // adj: if hit.result:                                                                    <L 13>
    wp::adj_mesh_query_ray(var_mesh, var_17, var_14, var_18, var_20, var_19, adj_mesh, adj_17, adj_14, adj_18, adj_20, adj_19);
    wp::adj_add(var_origin, var_16, adj_origin, adj_16, adj_17);
    wp::adj_mul(var_14, var_15, adj_14, adj_15, adj_16);
    // adj: hit=wp.mesh_query_ray(mesh,origin+direction*near,direction,4.)                    <L 12>
    wp::adj_mul(var_near_plane, var_12, adj_near_plane, adj_12, adj_15);
    // adj: near=near_plane*length                                                            <L 11>
    wp::adj_mul(var_R, var_13, adj_R, adj_13, adj_14);
    wp::adj_div(var_11, var_12, adj_11, adj_12, adj_13);
    wp::adj_length(var_11, var_12, adj_11, adj_12);
    // adj: length=wp.length(local);direction=R*(local/length)                                <L 8>
    wp::adj_vec_t(var_5, var_9, var_10, adj_5, adj_9, adj_10, adj_11);
    wp::adj_div(var_8, var_fy, var_9, adj_8, adj_fy, adj_9);
    wp::adj_neg(var_7, adj_7, adj_8);
    wp::adj_sub(var_6, var_cy, adj_6, adj_cy, adj_7);
    wp::adj_float(var_2, adj_2, adj_6);
    wp::adj_div(var_4, var_fx, var_5, adj_4, adj_fx, adj_5);
    wp::adj_sub(var_3, var_cx, adj_3, adj_cx, adj_4);
    wp::adj_float(var_1, adj_1, adj_3);
    // adj: local=wp.vec3((float(u)-cx)/fx,-(float(v)-cy)/fy,-1.)                             <L 7>
    wp::adj_mod(var_0, var_width, adj_0, adj_width, adj_1);
    // adj: i=wp.tid();u=i%width;v=i//width                                                   <L 6>
    // adj: def ray_image(mesh:wp.uint64,colors:wp.array(dtype=wp.vec3),robot_faces:wp.array(dtype=wp.int32),width:int,height:int,fx:float,fy:float,cx:float,cy:float,origin:wp.vec3,R:wp.mat33,shaded:int,near_plane:float,depth:wp.array(dtype=float),rgb:wp.array(dtype=wp.vec3),robot_mask:wp.array(dtype=wp.int32)):  <L 5>
    return;
}



extern "C" {

// Python CPU entry points
WP_API void ray_image_9194dcf6_cpu_forward(
    wp::launch_bounds_t<1> *dim,
    wp_args_ray_image_9194dcf6 *_wp_args)
{
    wp::tile_shared_storage_t tile_mem;
#if defined(WP_ENABLE_TILES_IN_STACK_MEMORY)
    wp::shared_tile_storage = &tile_mem;
#endif

    for (size_t task_index = 0; task_index < dim->size; ++task_index)
    {
        ray_image_9194dcf6_cpu_kernel_forward(*dim, task_index, _wp_args);
    }
}

} // extern C



extern "C" {

WP_API void ray_image_9194dcf6_cpu_backward(
    wp::launch_bounds_t<1> *dim,
    wp_args_ray_image_9194dcf6 *_wp_args,
    wp_args_ray_image_9194dcf6 *_wp_adj_args)
{
    wp::tile_shared_storage_t tile_mem;
#if defined(WP_ENABLE_TILES_IN_STACK_MEMORY)
    wp::shared_tile_storage = &tile_mem;
#endif

    for (size_t task_index = 0; task_index < dim->size; ++task_index)
    {
        ray_image_9194dcf6_cpu_kernel_backward(*dim, task_index, _wp_args, _wp_adj_args);
    }
}

} // extern C

