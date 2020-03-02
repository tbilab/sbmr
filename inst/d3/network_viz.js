// !preview r2d3 data=new_sbm_network(edges = clements_pollinators,bipartite_edges = TRUE,edges_from_column = pollinator,edges_to_column = flower), options = list(color_col = 'block', shape_col = 'type'), container = 'div'

div.html('')
  .style('width', `${width}px`)
  .style('height', `${height}px`)
  .style('position', 'relative');

// Append the canvas
const canvas = div.append('canvas')
  .style('position', 'absolute')
  .style('bottom', 0)
  .style('width', `${width}px`)
  .style('height', `${height}px`)
  .attr('width', width*2)
  .attr('height', height*2);

const context = canvas.node().getContext('2d');
context.scale(2,2); // Makes things bigger so it looks good on retina

// Append the svg and padded g element
const svg = div.append('svg')
  .html('') // wipe svg content if need be
  .style('position', 'absolute')
  .style('bottom', 0)
  .style('width', `${width}px`)
  .style('height', `${height}px`)
  .attr('width', width)
  .attr('height', height)
  .attr("viewBox", [0, 0, width, height]);

// Get color and shape column names from options
const {color_col, shape_col} = options;

// Clean up data into the node and link format that d3 needs
const edge_cols = Object.keys(data.edges);
const links = HTMLWidgets.dataframeToD3(data.edges)
  .map(function(edge){return({source: edge[edge_cols[0]], target: edge[edge_cols[1]]})});


const nodes = HTMLWidgets.dataframeToD3(data.nodes);


// If we're missing values for our aesthetic mapping columns,  fill them in with constant values.
if(!nodes[0][color_col]){
  nodes.forEach(function(node){node[color_col] = color_col});
}
if(!nodes[0][shape_col]){
  nodes.forEach(function(node){node[shape_col] = shape_col});
}

// Try and detect if color column is a numerical continuous value
const unique_color_vals = unique(nodes.map(d => d[color_col]));
const color_is_continuous = (unique_color_vals.length > 10) && converts_to_numeric(unique_color_vals[0]);

// Color encodes the node's block
const Color = color_is_continuous
  ? d3.scaleLinear().range(["white", "steelblue"]).domain(d3.extent(unique_color_vals))
  : d3.scaleOrdinal().range(d3.schemeCategory10).domain(unique_color_vals);

const color_node = node => {
  const color_val = node[color_col];
  return Color(color_is_continuous ? +color_val : color_val);
};

// Shape encodes the node's type
const Shape = d3.scaleOrdinal()
  .range(d3.symbols)
  .domain(unique(nodes.map(d => d[shape_col])));
const draw_shape = node => d3.symbol().size(150)
  .type(Shape(node[shape_col]))();


const padding = 15;
const X = d3.scaleLinear()
  .range([padding, width-padding]);

const Y = d3.scaleLinear()
  .range([padding, height-padding]);


// Setup the simulation
// Optimized for disjoint graphs...
const simulation = d3.forceSimulation(nodes)
    .force("link", d3.forceLink(links).id(d => d.id))
    .force("charge", d3.forceManyBody())
    .force("x", d3.forceX(width / 2))
    .force("y", d3.forceY(height / 2))
    .force('collision', d3.forceCollide().radius(10));

// Optimized for fully connected graphs.
//const simulation = d3.forceSimulation(nodes)
//      .force("link", d3.forceLink(links).id(d => d.id))
//      .force("charge", d3.forceManyBody().strength(-5))
//      .force("center", d3.forceCenter(width / 2, height / 2));

let not_being_dragged = true;

// Setup the svg node components
const node = svg.append("g")
  .attr("stroke", "#fff")
  .selectAll("path.node")
  .data(nodes)
  .enter().append('path')
  .classed('node', true)
  .attr('fill', color_node)
  .attr('d', draw_shape)
  .call(drag(simulation));

// Canvas constants
// Set color of edges
context.strokeStyle = 'black';

const lowest_opacity = 0.05;
// Scale edge opacity based upon how many edges we have
context.globalAlpha = Math.max(d3.scaleLinear()
                                 .domain([0,10000])
                                 .range([0.3, lowest_opacity])(links.length), lowest_opacity);


// Kickoff simulation
simulation.on("tick", () => {

 if(not_being_dragged){
   X.domain(d3.extent(nodes.map(d => d.x)));
   Y.domain(d3.extent(nodes.map(d => d.y)));
 }

  // Clear canvas
  context.clearRect(0, 0, +canvas.attr('width'), +canvas.attr('height'));

  context.beginPath();
  links.forEach(d => {
    context.moveTo(X(d.source.x), Y(d.source.y));
    context.lineTo(X(d.target.x), Y(d.target.y));
  });

  // Draw to canvas
  context.stroke();

  // Update node positions
  node.attr('transform', d => `translate(${X(d.x)}, ${Y(d.y)})`);
});


function drag(simulation){
  function dragstarted(d) {
    if (!d3.event.active) simulation.alphaTarget(0.3).restart();
    d.fx = d.x;
    d.fy = d.y;
    not_being_dragged = false;
  }

  function dragged(d) {
    d.fx = d3.event.x;
    d.fy = d3.event.y;
  }

  function dragended(d) {
    if (!d3.event.active) simulation.alphaTarget(0);
    d.fx = null;
    d.fy = null;
    not_being_dragged = true;
  }

  return d3.drag()
      .on("start", dragstarted)
      .on("drag", dragged)
      .on("end", dragended);
}

function converts_to_numeric(val){
  return !isNaN(+val);
}

function unique (vals){
  return [...new Set(vals)];
}


