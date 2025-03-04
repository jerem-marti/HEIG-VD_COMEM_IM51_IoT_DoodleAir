import  { WebSocketServer } from 'ws';

export default class WebSocketServerOrigin extends WebSocketServer {

  constructor(options, callback) {
    // Ensure path is set with a default value if not provided
    if (!options.path) {
      options.path = '/ws';
    }
    
    super(options, callback);
    
    // origins: string | string[]
    if (!this.options?.origins) {
      throw new Error('Missing origins option');
    }
    if (typeof this.options.origins !== 'string' && !Array.isArray(this.options.origins)) {
      throw new Error('Invalid origins option');
    }
    if (typeof this.options.origins === 'string') {
      this.options.origins = [this.options.origins];
    }
    // Max clients
    if (!this.options?.maxNbOfClients) {
      throw new Error('Missing maxNbOfClients option');
    }
    // Must be a int greater than 0
    if (!Number.isInteger(this.options.maxNbOfClients) || this.options.maxNbOfClients <= 0) {
      throw new Error('Invalid maxNbOfClients option');
    }
    
    this.log(`WebSocket server configured with path: ${this.options.path}`);
  }

  log(message) {
    if (!this.options?.verbose) return;
    console.log(`[WSS] ${message}`);
  }

  handleUpgrade(request, socket, head, callback) {
    // Log detailed request information
    this.log(`Received upgrade request for path: ${request.url}`);
    this.log(`Headers: ${JSON.stringify(request.headers)}`);
    
    // Check if the path matches what we expect
    const configuredPath = this.options.path || '/ws';
    const requestPath = request.url.split('?')[0]; // Remove query parameters
    
    this.log(`Comparing request path: "${requestPath}" with configured path: "${configuredPath}"`);
    
    // Be lenient with path matching - accept if the path starts with the configured path
    // or if the configured path is a prefix of the request path
    if (configuredPath !== '/' && requestPath !== configuredPath && !requestPath.startsWith(configuredPath + '/')) {
      this.log(`Path mismatch: ${requestPath} does not match ${configuredPath}`);
      socket.write('HTTP/1.1 404 Not Found\r\n\r\n');
      socket.destroy();
      return;
    }
    
    // Validate origin
    if (!this.checkOrigin(request.headers?.origin)) {
      this.log(`Origin validation failed for: ${request.headers?.origin}`);
      socket.write('HTTP/1.1 403 Forbidden\r\n\r\n');
      socket.destroy();
      return;
    }
    
    // Check if server is full
    if (this.clients.size >= this.options.maxNbOfClients) {
      this.log('Server is full');
      socket.write('HTTP/1.1 503 Service Unavailable\r\n\r\n');
      socket.destroy();
      return;
    }

    this.log(`Upgrading connection to WebSocket for path: ${request.url}`);
    return super.handleUpgrade(request, socket, head, callback);
  }

  checkOrigin(origin) {
    // If origin is not provided, accept the connection
    // This is common for ESP32 and other embedded devices
    if (!origin) {
      this.log('No origin header provided, accepting connection');
      return true;
    }
    
    // If any origin is allowed, accept the connection
    if (this.options.origins.includes('*')) {
      this.log(`Origin ${origin} accepted due to wildcard`);
      return true;
    }
    
    // Otherwise, check if the origin is allowed
    for (const allowedOrigin of this.options.origins) {
      if (allowedOrigin === '*') return true;
      if (!allowedOrigin.startsWith('http')) { // if allowedOrigin accept any protocol
        origin = origin.replace(/(https?:\/\/)?/, '');
      }
      if (!allowedOrigin.match(/:\d+$/)) { // if allowedOrigin accept any port
        origin = origin.replace(/:\d+$/, '');
      }
      if (origin === allowedOrigin) {
        this.log(`Origin ${origin} matched allowed origin ${allowedOrigin}`);
        return true;
      }
    }
    
    this.log(`Origin ${origin} did not match any allowed origins`);
    return false;
  }

}