import { mat4 } from 'https://cdn.skypack.dev/gl-matrix';

function createSphere(radius, latBands, longBands) {
    const positions = [];
    const texcoords = [];
    const normals = [];
    const indices = [];

    for (let lat = 0; lat <= latBands; lat++) {
        const theta = (lat * Math.PI) / latBands;
        const sinTheta = Math.sin(theta);
        const cosTheta = Math.cos(theta);

        for (let lon = 0; lon <= longBands; lon++) {
            const phi = (lon * 2 * Math.PI) / longBands;
            const sinPhi = Math.sin(phi);
            const cosPhi = Math.cos(phi);

            const x = cosPhi * sinTheta;
            const y = cosTheta;
            const z = sinPhi * sinTheta;

            const u = 1 - (lon / longBands);
            const v = 1 - (lat / latBands);

            positions.push(radius * x, radius * y, radius * z);
            texcoords.push(u, v);
            normals.push(x, y, z); // As normais são iguais às posições normalizadas
        }
    }

    for (let lat = 0; lat < latBands; lat++) {
        for (let lon = 0; lon < longBands; lon++) {
            const first = (lat * (longBands + 1)) + lon;
            const second = first + longBands + 1;

            indices.push(first, second, first + 1);
            indices.push(second, second + 1, first + 1);
        }
    }

    return {
        positions: new Float32Array(positions),
        texcoords: new Float32Array(texcoords),
        normals: new Float32Array(normals),
        indices: new Uint16Array(indices),
    };
}

async function loadTexture(gl, url) {
    const texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);

    // Carregar textura temporária até que a imagem seja carregada
    const level = 0;
    const internalFormat = gl.RGBA;
    const width = 1;
    const height = 1;
    const border = 0;
    const srcFormat = gl.RGBA;
    const srcType = gl.UNSIGNED_BYTE;
    const pixel = new Uint8Array([255, 255, 255, 255]); // Branco
    gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, width, height, border, srcFormat, srcType, pixel);

    const image = new Image();
    image.src = url;
    await new Promise((resolve) => {
        image.onload = () => {
            gl.bindTexture(gl.TEXTURE_2D, texture);
            gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, srcFormat, srcType, image);
            gl.generateMipmap(gl.TEXTURE_2D);
            resolve();
        };
    });
    return texture;
}

function createShader(gl, type, source) {
    const shader = gl.createShader(type);
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.error('Erro ao compilar shader:', gl.getShaderInfoLog(shader));
        gl.deleteShader(shader);
        return null;
    }
    return shader;
}

function createProgram(gl, vertexShader, fragmentShader) {
    const program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        console.error('Erro ao linkar programa:', gl.getProgramInfoLog(program));
        gl.deleteProgram(program);
        return null;
    }
    return program;
}

async function main() {
    const canvas = document.querySelector("#canvas");
    const gl = canvas.getContext("webgl2");
    if (!gl) {
        console.error("WebGL2 não suportado");
        return;
    }

    const vertexShaderSource = `#version 300 es
    in vec4 a_position;
    in vec2 a_texcoord;
    in vec3 a_normal;

    uniform mat4 u_modelViewMatrix;
    uniform mat4 u_projectionMatrix;
    uniform mat4 u_normalMatrix;

    out vec2 v_texcoord;
    out vec3 v_normal;
    out vec3 v_position;

    void main() {
        gl_Position = u_projectionMatrix * u_modelViewMatrix * a_position;
        v_texcoord = a_texcoord;
        v_normal = mat3(u_normalMatrix) * a_normal;
        v_position = (u_modelViewMatrix * a_position).xyz;
    }`;

    const fragmentShaderSource = `#version 300 es
    precision highp float;

    in vec2 v_texcoord;
    in vec3 v_normal;
    in vec3 v_position;

    uniform sampler2D u_texture;
    uniform vec3 u_lightPosition;
    uniform vec3 u_ambientLight;
    uniform bool u_useLighting;

    out vec4 outColor;

    void main() {
        vec4 texColor = texture(u_texture, v_texcoord);

        if (u_useLighting) {
            vec3 normal = normalize(v_normal);
            vec3 lightDir = normalize(u_lightPosition - v_position);
            float diff = max(dot(normal, lightDir), 0.0);

            // Iluminação ambiente
            vec3 ambient = u_ambientLight * texColor.rgb;

            // Iluminação difusa
            vec3 diffuse = diff * texColor.rgb;

            vec3 finalColor = ambient + diffuse;

            outColor = vec4(finalColor, texColor.a);
        } else {
            // Renderizar sem iluminação (para o fundo estrelado)
            outColor = texColor;
        }
    }`;

    const vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    const fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);
    const program = createProgram(gl, vertexShader, fragmentShader);
    gl.useProgram(program);

    // Carregar texturas
    const sunTexture = await loadTexture(gl, 'textures/sun.jpg');
    const earthTexture = await loadTexture(gl, 'textures/earth.jpg');

    // Criar esferas para os planetas e o Sol
    const sunSphere = createSphere(3, 30, 30);
    const earthSphere = createSphere(1, 30, 30);

    // Localizações de atributos e uniformes
    const a_position = gl.getAttribLocation(program, 'a_position');
    const a_texcoord = gl.getAttribLocation(program, 'a_texcoord');
    const a_normal = gl.getAttribLocation(program, 'a_normal');

    const u_modelViewMatrix = gl.getUniformLocation(program, 'u_modelViewMatrix');
    const u_projectionMatrix = gl.getUniformLocation(program, 'u_projectionMatrix');
    const u_normalMatrix = gl.getUniformLocation(program, 'u_normalMatrix');

    const u_texture = gl.getUniformLocation(program, 'u_texture');
    const u_lightPosition = gl.getUniformLocation(program, 'u_lightPosition');
    const u_ambientLight = gl.getUniformLocation(program, 'u_ambientLight');
    const u_useLighting = gl.getUniformLocation(program, 'u_useLighting');

    // Configurar posição da luz (Sol no centro)
    gl.uniform3fv(u_lightPosition, [0.0, 0.0, 0.0]);

    // Iluminação ambiente 
    const ambientLight = [0.5, 0.5, 0.5];
    gl.uniform3fv(u_ambientLight, ambientLight);

    // Ativar teste de profundidade
    gl.enable(gl.DEPTH_TEST);

    // Matriz de projeção
    const fieldOfView = 45 * Math.PI / 180;   // em radianos
    const aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
    const near = 0.1;
    const far = 1000.0;
    const projectionMatrix = mat4.create();
    mat4.perspective(projectionMatrix, fieldOfView, aspect, near, far);

    // Definir a matriz de projeção no shader
    gl.uniformMatrix4fv(u_projectionMatrix, false, projectionMatrix);

    // Animação
    let lastTime = 0;
    function render(time) {
        time *= 0.001;  // converter para segundos
        const deltaTime = time - lastTime;
        lastTime = time;

        // Ajustar o tamanho do canvas
        gl.canvas.width = gl.canvas.clientWidth;
        gl.canvas.height = gl.canvas.clientHeight;
        gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // Configurar matriz de visão
        const cameraPosition = [0, 0, 70];
        const target = [0, 0, 0];
        const up = [0, 1, 0];
        const viewMatrix = mat4.create();
        mat4.lookAt(viewMatrix, cameraPosition, target, up);

        // Parâmetros das órbitas dos planetas
        const earthOrbitRadius = 15;
        const earthOrbitSpeed = 1;

        // Calcular posições orbitais
        const earthAngle = time * earthOrbitSpeed;

        const earthPosition = [
            earthOrbitRadius * Math.cos(earthAngle),
            0,
            earthOrbitRadius * Math.sin(earthAngle)
        ];

        // Rotações dos planetas em torno de seus próprios eixos
        const sunRotation = time * 0.5;
        const earthRotation = time * 2;

        // Desenhar os planetas com iluminação
        gl.uniform1i(u_useLighting, 1); // Ativar iluminação

        // Desenhar o Sol
        drawPlanet(sunSphere, sunTexture, [0, 0, 0], [3, 3, 3], sunRotation, viewMatrix);

        // Desenhar a Terra
        drawPlanet(earthSphere, earthTexture, earthPosition, [1, 1, 1], earthRotation, viewMatrix);

        requestAnimationFrame(render);
    }

    // Função para desenhar um planeta
    function drawPlanet(sphere, texture, position, scale, rotation, viewMatrix) {
        const { positions, texcoords, normals, indices } = sphere;

        // Buffer de posições
        const positionBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, positions, gl.STATIC_DRAW);
        gl.enableVertexAttribArray(a_position);
        gl.vertexAttribPointer(a_position, 3, gl.FLOAT, false, 0, 0);

        // Buffer de coordenadas de textura
        const texcoordBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, texcoordBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, texcoords, gl.STATIC_DRAW);
        gl.enableVertexAttribArray(a_texcoord);
        gl.vertexAttribPointer(a_texcoord, 2, gl.FLOAT, false, 0, 0);

        // Buffer de normais
        const normalBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, normalBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, normals, gl.STATIC_DRAW);
        gl.enableVertexAttribArray(a_normal);
        gl.vertexAttribPointer(a_normal, 3, gl.FLOAT, false, 0, 0);

        // Buffer de índices
        const indexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indices, gl.STATIC_DRAW);

        // Construir a matriz de modelo
        const modelMatrix = mat4.create();
        mat4.translate(modelMatrix, modelMatrix, position);
        mat4.rotateY(modelMatrix, modelMatrix, rotation);
        mat4.scale(modelMatrix, modelMatrix, scale);

        // Calcular matriz de modelo-visão
        const modelViewMatrix = mat4.create();
        mat4.multiply(modelViewMatrix, viewMatrix, modelMatrix);

        // Calcular matriz normal
        const normalMatrix = mat4.create();
        mat4.invert(normalMatrix, modelViewMatrix);
        mat4.transpose(normalMatrix, normalMatrix);

        // Definir uniformes de matriz
        gl.uniformMatrix4fv(u_modelViewMatrix, false, modelViewMatrix);
        gl.uniformMatrix4fv(u_normalMatrix, false, normalMatrix);

        // Aplicar textura
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.uniform1i(u_texture, 0);

        gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT, 0);
    }

    gl.clearColor(0.0, 0.0, 0.0, 1.0);  // Fundo preto para o espaço

    requestAnimationFrame(render);
}

main();
