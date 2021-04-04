#version 330 core

out vec4 frag_color;

struct AmbientLight {
    bool enabled;
    vec3 ambient;
};

struct PointLight {
    bool enabled;
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

struct DirectionalLight {
    bool enabled;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 world_position;
in vec3 world_normal;
in vec2 tex_coord;

uniform bool ambient_texture_flag;
uniform bool diffuse_texture_flag;
uniform bool specular_texture_flag;

uniform vec3 camera_position;

uniform Material material; // material properties of the object
uniform AmbientLight ambient_light;
uniform PointLight point_light;
uniform DirectionalLight directional_light;
vec3 CalcAmbientLight();
vec3 CalcPointLight(vec3 normal, vec3 view_dir);
vec3 CalcDirectionalLight(vec3 normal, vec3 view_dir);

uniform sampler2D diffuse_texture;
uniform sampler2D ambient_texture;
uniform sampler2D specular_texture;
uniform sampler2D shadow_texture;
uniform mat4 world_to_light_ndc_matrix;
uniform bool draw_pcf;

void main() {
    vec3 normal = normalize(world_normal);
    vec3 view_dir = normalize(camera_position - world_position);

    frag_color = vec4(0.0);

    if (ambient_light.enabled) {
        frag_color += vec4(CalcAmbientLight(), 1.0);
    }

    if (point_light.enabled) {
        frag_color += vec4(CalcPointLight(normal, view_dir), 1.0);
    }

    if (directional_light.enabled) {
        frag_color += vec4(CalcDirectionalLight(normal, view_dir), 1.0);
    }
}

vec3 GetAmbientColor() {
    if (ambient_texture_flag) {
        return texture(ambient_texture, tex_coord).xyz;
    } else {
        return material.ambient;
    }
}

vec3 GetDiffuseColor() {
    if (diffuse_texture_flag) {
        return texture(diffuse_texture, tex_coord).xyz;
    } else {
        return material.diffuse;
    }
}

vec3 GetSpecularColor() {
    if (specular_texture_flag) {
        return texture(specular_texture, tex_coord).xyz;
    } else {
        return material.specular;
    }
}
vec3 CalcAmbientLight() {
    return ambient_light.ambient * GetAmbientColor();
}

vec3 CalcPointLight(vec3 normal, vec3 view_dir) {
    PointLight light = point_light;
    vec3 light_dir = normalize(light.position - world_position);

    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_color = diffuse_intensity * light.diffuse * GetDiffuseColor();

    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_intensity = pow(
        max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = specular_intensity *
        light.specular * GetSpecularColor();

    float distance = length(light.position - world_position);
    float attenuation = 1.0 / (light.attenuation.x +
        light.attenuation.y * distance +
        light.attenuation.z * (distance * distance));

    return attenuation * (diffuse_color + specular_color);
}

vec3 CalcDirectionalLight(vec3 normal, vec3 view_dir) {
    DirectionalLight light = directional_light;
    vec3 light_dir = normalize(-light.direction);
    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_color = diffuse_intensity * light.diffuse * GetDiffuseColor();

    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_intensity = pow(
        max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = specular_intensity *
        light.specular * GetSpecularColor();

    vec3 final_color = diffuse_color+specular_color;

    // From Piazza
//    1. Transform world_position from world to NDC coordinates
//    2. Transform from NDC coords to texture coords (ndc + 1)/2
//    3. Calculate the depth of this coord
//    4. Calculate the depth of the occluding object
//    5. If occluder_depth + bias < this_depth --> turn the final_color black

     //origianl hard shadow method
    if (draw_pcf == false){
        vec4 pos = world_to_light_ndc_matrix * vec4(world_position, 1.0f);
        vec2 point_shadow_tex = vec2((pos.x + 1.0f)/2.0f, (pos.y + 1.0f)/2.0f);
        float map_depth = texture(shadow_texture, point_shadow_tex).r;
        float cur_depth = (pos.z + 1.0f)/2.0f;
        if (map_depth + 0.0001f < cur_depth) {
            final_color = vec3(0.0f);
        }
    }
    else {// similar to https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
        float shadow_ratio = 0.0f;
        vec4 pos = world_to_light_ndc_matrix * vec4(world_position, 1.0f);
        vec2 point_shadow_tex = vec2((pos.x + 1.0f)/2.0f, (pos.y + 1.0f)/2.0f);
        float cur_depth = (pos.z + 1.0f)/2.0f;
        for (int x = -4; x <= 4; ++x)
        {
            for (int y = -4; y <= 4; ++y)
            {
                vec2 new = point_shadow_tex + vec2(float(x)/(4096.f), float(y)/(4096.f));
                if ((new.x>0.f)&&(new.x<1.f)&&(new.y>0.f)&&(new.y<1.f)){
                    float pcfDepth = texture(shadow_texture, new).r;
                    if (pcfDepth+0.0001f <cur_depth){
                        shadow_ratio +=1.0f;
                    }
                }
                else {
                    shadow_ratio+=0.f;
                }
            }
        }
        shadow_ratio /= (81.0f);
        shadow_ratio *=0.5f;// make it 0-0.5 to look better
        final_color = final_color*(1-shadow_ratio);
    }

    return final_color;
}

//#version 330 core
//
//out vec4 frag_color;
//
//struct AmbientLight {
//    bool enabled;
//    vec3 ambient;
//};
//
//struct PointLight {
//    bool enabled;
//    vec3 position;
//    vec3 diffuse;
//    vec3 specular;
//    vec3 attenuation;
//};
//
//struct DirectionalLight {
//    bool enabled;
//    vec3 direction;
//    vec3 diffuse;
//    vec3 specular;
//};
//struct Material {
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//    float shininess;
//};
//
//in vec3 world_position;
//in vec3 world_normal;
//in vec2 tex_coord;
//
//uniform float BIAS = 0.0001f;
//
//uniform vec3 camera_position;
//
//uniform sampler2D ambient_texture;
//uniform sampler2D diffuse_texture;
//uniform sampler2D specular_texture;
//
//uniform bool ambient_texture_flag;
//uniform bool diffuse_texture_flag;
//uniform bool specular_texture_flag;
//
//uniform bool draw_shadow = true;
//uniform bool draw_pcf = false;
//uniform sampler2D shadow_texture;
//uniform mat4 world_to_light_ndc_matrix;
//
//uniform Material material; // material properties of the object
//uniform AmbientLight ambient_light;
//uniform PointLight point_light;
//uniform DirectionalLight directional_light;
//vec3 CalcAmbientLight();
//vec3 CalcPointLight(vec3 normal, vec3 view_dir);
//vec3 CalcDirectionalLight(vec3 normal, vec3 view_dir);
//
//void main() {
//    vec3 normal = normalize(world_normal);
//    vec3 view_dir = normalize(camera_position - world_position);
//
//    frag_color = vec4(0.0);
//
//    if (ambient_light.enabled) {
//        frag_color += vec4(CalcAmbientLight(), 1.0);
//    }
//
//    if (point_light.enabled) {
//        frag_color += vec4(CalcPointLight(normal, view_dir), 1.0);
//    }
//
//    if (directional_light.enabled) {
//        frag_color += vec4(CalcDirectionalLight(normal, view_dir), 1.0);
//    }
//}
//
//vec3 GetAmbientColor() {
//    if (ambient_texture_flag) {
//        return texture(ambient_texture, tex_coord).xyz;
//    } else {
//        return material.ambient;
//    }
//}
//
//vec3 GetDiffuseColor() {
//    if (diffuse_texture_flag) {
//        return texture(diffuse_texture, tex_coord).xyz;
//    } else {
//        return material.diffuse;
//    }
//}
//
//vec3 GetSpecularColor() {
//    if (specular_texture_flag) {
//        return texture(specular_texture, tex_coord).xyz;
//    } else {
//        return material.specular;
//    }
//}
//
//vec3 CalcAmbientLight() {
//    return ambient_light.ambient * GetAmbientColor();
//}
//
////float CalcPCFRatio(vec2 shadow_tex_coord, int window_length_x, int window_length_y, float this_depth) {   // shadow_tex_coord in [0, 1]
////
////    float factor = 0.f;
////    vec2 shadow_tex_size = textureSize(shadow_texture, 0);
////    float offset_x = 1.0f/shadow_tex_size.x;
////    float offset_y = 1.0f/shadow_tex_size.y;
////
////    for (int y = -window_length_y/2; y<=window_length_y/2; y++) {
////        for (int x = -window_length_x/2; x<=window_length_x/2; x++) {
////            vec2 tmp_coord = vec2(shadow_tex_coord.x + x*offset_x, shadow_tex_coord.y + y*offset_y);
////            factor += ( (tmp_coord.x > 0.0f && tmp_coord.x < 1.0f
////            && tmp_coord.y > 0.0f && tmp_coord.y < 1.0f) && (texture(shadow_texture, tmp_coord).r + BIAS < this_depth) )? 0.0f : 1.0f;
////        }
////    }
////    return 0.5f + factor/float(2*window_length_x*window_length_y);
////}
//
//vec3 CalcPointLight(vec3 normal, vec3 view_dir) {
//    PointLight light = point_light;
//    vec3 light_dir = normalize(light.position - world_position);
//
//    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
//    vec3 diffuse_color = diffuse_intensity * light.diffuse * GetDiffuseColor();
//
//    vec3 reflect_dir = reflect(-light_dir, normal);
//    float specular_intensity = pow(
//    max(dot(view_dir, reflect_dir), 0.0), material.shininess);
//    vec3 specular_color = specular_intensity *
//    light.specular * GetSpecularColor();
//
//    float distance = length(light.position - world_position);
//    float attenuation = 1.0 / (light.attenuation.x +
//    light.attenuation.y * distance +
//    light.attenuation.z * (distance * distance));
//
//    vec3 final_color = attenuation * (diffuse_color + specular_color);
//
//    if (draw_shadow){
//        vec4 point_ndc = world_to_light_ndc_matrix * vec4(world_position, 1.0);
//
//        float this_depth = (point_ndc.z + 1.0f)/2.0f;
//
//        vec2 point_shadow_tex = vec2((point_ndc.x + 1.0f)/2.0f, (point_ndc.y + 1.0f)/2.0f);
//
//        if (draw_pcf){
//            final_color = 0.5 * final_color;
//        } else {
//            float occluder_depth = texture(shadow_texture, point_shadow_tex).r;
//            if (occluder_depth + BIAS < this_depth) {
//                final_color = 0.5f*final_color;
//            }
//        }
//    }
//
//    return final_color;
//}
//
//vec3 CalcDirectionalLight(vec3 normal, vec3 view_dir) {
//
//    DirectionalLight light = directional_light;
//    vec3 light_dir = normalize(-light.direction);
//    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
//    vec3 diffuse_color = diffuse_intensity * light.diffuse * GetDiffuseColor();
//
//    vec3 reflect_dir = reflect(-light_dir, normal);
//    float specular_intensity = pow(
//    max(dot(view_dir, reflect_dir), 0.0), material.shininess);
//    vec3 specular_color = specular_intensity *
//    light.specular * GetSpecularColor();
//
//    vec3 final_color = diffuse_color + specular_color;
//
//    if (draw_shadow){
//        vec4 point_ndc = world_to_light_ndc_matrix * vec4(world_position, 1.0);
//
//        float this_depth = (point_ndc.z + 1.0f)/2.0f;
//
//        vec2 point_shadow_tex = vec2((point_ndc.x + 1.0f)/2.0f, (point_ndc.y + 1.0f)/2.0f);
//
//        if (draw_pcf){
//            final_color = 0.5 * final_color;
//        } else {
//            float occluder_depth = texture(shadow_texture, point_shadow_tex).r;
//            if (occluder_depth + BIAS < this_depth) {
//                final_color = 0.5f*final_color;
//            }
//        }
//    }
//    return final_color;
//}
//


