// Helper: calculates the shortest distance from point p to the line segment ab.
float sdSegment(vec2 p, vec2 a, vec2 b) {
  // Compute the vector from a to p, and from a to b.
  vec2 pa = p - a, ba = b - a;
  // Project pa onto ba and clamp the result to lie between 0 and 1.
  float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
  // Return the distance from p to the closest point on the segment.
  return length(pa - ba * h);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord){
  // Normalize pixel coordinates (0..1) and center them at (0,0).
  vec2 uv = fragCoord.xy / iResolution.xy;
  vec2 c = uv - 0.5; 
  // Correct the x coordinate for the screen's aspect ratio.
  c.x *= iResolution.x / iResolution.y;
  
  // Define constants:
  // PI: constant for π.
  // thick: line thickness for drawing arcs and gap lines.
  // baseR: starting radius for the outermost circle.
  const float PI = 3.14159265359, thick = 0.005, baseR = 0.5;
  // iter: total number of circles (and gap lines) to draw.
  int iter = 20;
  // dur: total duration for each iteration (arc + gap phases).
  // arcDur: portion of dur used to animate the arc.
  // gapDur: portion of dur used to animate the connecting gap line.
  // gapR: how much the radius decreases per iteration.
  float dur = .5, arcDur = dur * 0.8, gapDur = dur * 0.2, gapR = baseR / float(iter);
  // tot: total animation duration (for all iterations).
  // T: current time modulo tot, so the animation loops.
  float tot = dur * float(iter), T = mod(iTime, tot);
  // Compute the distance from the current pixel to the center.
  float dist = length(c);
  // Compute the pixel's polar angle with an offset so that 0 radians is at the top.
  float aPix = mod(atan(c.x, c.y) - PI/2.0 + 2.0*PI, 2.0*PI);
  
  // col accumulates the color contribution of all arcs and gap lines.
  float col = 0.0;
  // s is the starting angle for the current iteration.
  float s = 0.0;
  
  // Loop over each iteration (each circle with its connecting gap line).
  for (int i = 0; i < iter; i++){
    // t0: the start time for this iteration.
    float t0 = float(i) * dur;
    // lt: local time within the current iteration, clamped between 0 and dur.
    float lt = clamp(T - t0, 0.0, dur);
    // rnd: pseudo-random value for this iteration to vary the arc length.
    float rnd = fract(sin(float(i) * 12.9898) * 43758.5453);
    // curArc: the total arc angle for this iteration, varying between PI and 2π.
    float curArc = mix(PI, 2.0 * PI, rnd);
    // d: drawing direction for this iteration; alternates between 1 and -1.
    float d = (mod(float(i), 2.0) < 0.5) ? 1.0 : -1.0;
    // r: the radius for this iteration's circle.
    float r = baseR - float(i) * gapR;
    // sweep: current sweep angle of the arc; it animates from 0 to curArc over arcDur.
    float sweep = lt < arcDur ? (lt / arcDur * curArc) : curArc;
    // diff: angular difference between the pixel's angle and the starting angle,
    // adjusted for the drawing direction.
    float diff = (d > 0.0) ? mod(aPix - s, 2.0 * PI) : mod(s - aPix, 2.0 * PI);
    // arcInd: indicator (0 or 1) if the pixel is within the current arc's angular range.
    float arcInd = step(diff, sweep);
    // edge: a smooth circular band (using smoothstep) defining the arc's radial edge.
    float edge = smoothstep(r - thick, r, dist) - smoothstep(r, r + thick, dist);
    // contrib: contribution from the arc part, only active where arcInd is 1.
    float contrib = edge * arcInd;
    
    // LOCK the arc's final angle once the arc phase is complete.
    // s_locked will be the final angle at which the arc ends.
    float s_locked = s;
    if(lt >= arcDur)
      s_locked = -s-d*curArc ;//-d*curArc ; //PI/2.+ s + d*curArc;
      
    // Draw the gap (connecting) line if there's a next circle.
    if(i < iter - 1 && lt > arcDur){
      // gp: progress through the gap phase (ranging from 0 to 1).
      float gp = (lt - arcDur) / gapDur;
      // r_next: the radius of the next circle (next iteration).
      float r_next = baseR - float(i + 1) * gapR;
      // Compute the endpoints of the gap line using the locked angle:
      // P: endpoint of the current arc at radius r.
      vec2 P = vec2(r * cos(s_locked), r * sin(s_locked));
      // Q: starting point of the next arc at radius r_next.
      vec2 Q = vec2(r_next * cos(s_locked), r_next * sin(s_locked));
      // Animate the gap line: it grows from P (when gp = 0) to Q (when gp = 1)
      // and then remains fixed.
      vec2 Q_anim = mix(P, Q, gp);
      // Calculate the distance from the current pixel to the animated line segment.
      float dLine = sdSegment(c, P, Q_anim);
      // lineMask: defines the gap line's appearance; pixels within a distance
      // from the line (using a threshold based on thick) contribute to the line.
      float lineMask = 1.0 - smoothstep(thick * 0.5, thick, dLine);
      // Add the gap line's contribution to the current iteration's drawing.
      contrib += lineMask;
    }
    
    // Accumulate the contribution from this iteration into the overall color.
    col += contrib;
    // Once the full duration for this iteration has passed, update the global
    // starting angle for the next iteration using the locked arc angle.
    if(T >= t0 + dur - 0.001)
      s = s + d * curArc;
  }
  
  // Clamp the final color value and output the resulting fragment color.
  col = clamp(col, 0.0, 1.0);
  fragColor = vec4(mix(vec3(1.0), vec3(0.0), col), 1.0);
}
