# MedBox — Web Application

## Structure

```
web/
├── frontend/           Static HTML/CSS/JS (deployed to Netlify)
│   ├── index.html      Dashboard
│   ├── medications.html Medication CRUD
│   ├── history.html    Dispense history
│   ├── settings.html   System settings
│   ├── css/style.css   Design system
│   └── js/             Client-side logic
│
├── netlify/
│   ├── functions/      Serverless API functions
│   └── ...
│
└── netlify.toml        Netlify config & API redirects
```

## Architecture

```
Browser → Netlify CDN → Static Frontend
                ↓
        /api/* routes → Netlify Functions → Database
                                    ↓
                               ntfy (notifications)
                                    ↓
                               ESP32 (sync endpoint)
```

## Deployment
1. Connect the `web/` directory to a Netlify site.
2. Set environment variables in the Netlify dashboard.
3. Deploy — frontend and functions are deployed together.

## Local Development
```bash
cd web
npx netlify-cli dev
```
