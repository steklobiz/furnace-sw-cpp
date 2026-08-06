Problem	How should:
Log be distributed to consumers?

Chosen direction:
Push to USART (immediate) + Store in ring buffer (for TUI pull later)

Rationale:
USART needs real-time streaming. TUI needs historical access. Different timing requirements
