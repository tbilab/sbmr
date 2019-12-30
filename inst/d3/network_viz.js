// !preview r2d3 data=sim_basic_block_network(3, 20), options = list(color_col = 'block', shape_col = 'type'), container = 'div'

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
const links = HTMLWidgets.dataframeToD3(data.edges)
  .map(function(edge){return({source: edge.from, target: edge.to})});
const nodes = HTMLWidgets.dataframeToD3(data.nodes);


// If we're missing values for our aesthetic mapping columns,  fill them in with constant values.
if(!nodes[0][color_col]){
  nodes.forEach(function(node){node[color_col] = color_col});
}
if(!nodes[0][shape_col]){
  nodes.forEach(function(node){node[shape_col] = shape_col});
}

// Color encodes the node's block
const Color = d3.scaleOrdinal(d3.schemeCategory10, unique(nodes.map(d => d[color_col])));
const color_node = node => Color(node[color_col]);

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
const simulation = d3.forceSimulation(nodes)
      .force("link", d3.forceLink(links).id(d => d.id))
      .force("charge", d3.forceManyBody())
      .force("center", d3.forceCenter(width / 2, height / 2));

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

const lowest_opacity = 0.1;
// Scale edge opacity based upon how many edges we have
context.globalAlpha = Math.max(d3.scaleLinear().domain([0,5000]).range([0.5, lowest_opacity])(links.length), lowest_opacity);


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


function unique (vals){
  return [...new Set(vals)];
}


