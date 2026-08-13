# MedBox — Frontend

Plain HTML/CSS/JS web application. No build step required.

## Pages

| Page | Purpose |
|------|---------|
| `index.html` | Dashboard — today's schedule, system status |
| `medications.html` | Add/edit/delete medications and schedules |
| `history.html` | Dispense log / medication history |
| `settings.html` | Device, notification, and module settings |

## JS Modules

| File | Purpose |
|------|---------|
| `js/api.js` | Fetch wrappers for all backend API calls |
| `js/app.js` | Toast notifications, dashboard rendering |
| `js/medications.js` | Medication form handling, CRUD |
| `js/schedule.js` | Schedule display utilities |
| `js/history.js` | History table rendering |

## Deployment
Deploy the entire `frontend/` directory as a Netlify static site.
API calls are routed to Netlify Functions via `netlify.toml` redirects.
